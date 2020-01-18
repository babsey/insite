#include "http_server.hpp"
#include "data_storage.hpp"

namespace insite {

HttpServer::HttpServer(web::http::uri address, DataStorage* storage)
    : http_listener_{address}, storage_(storage) {
  http_listener_.support([this](web::http::http_request request) {
    if (request.method() == "GET" &&
        request.relative_uri().path() == "/spikes") {
      request.reply(GetSpikes(request));
    } else if (request.method() == "GET" &&
        request.relative_uri().path() == "/multimeter_measurement") {
      request.reply(GetMultimeterMeasurement(request));
    } else {
      std::cerr << "Invalid request: " << request.to_string() << "\n";
      request.reply(web::http::status_codes::NotFound);
    }
  });

  http_listener_.open().wait();
  std::cout << "HTTP server is listening...\n";
}

web::http::http_response HttpServer::GetSpikes(
    const web::http::http_request& request) {
  const auto parameters = web::uri::split_query(request.request_uri().query());

  web::http::http_response response(web::http::status_codes::OK);
  const auto spikes = storage_->GetSpikes();
  auto spikes_begin = spikes.begin();
  auto spikes_end = spikes.end();

  const auto from = parameters.find("from");
  const auto to = parameters.find("to");

  const auto spike_happened_before = [](const Spike& spike,
                                        double simulation_time) {
    return spike.simulation_time < simulation_time;
  };

  if (from != parameters.end()) {
    const auto from_number = std::stoll(from->second);
    spikes_begin = std::lower_bound(spikes.begin(), spikes.end(), from_number,
                                    spike_happened_before);
  }

  if (to != parameters.end()) {
    const auto to_number = std::stoll(to->second);
    spikes_end = std::lower_bound(spikes.begin(), spikes.end(), to_number,
                                  spike_happened_before);
  }

  const auto element_count = spikes_end - spikes_begin;
  web::json::value gids = web::json::value::array(element_count);
  web::json::value simulation_times = web::json::value::array(element_count);

  {
    size_t index = 0;
    for (auto spike = spikes_begin; spike != spikes_end; ++spike, ++index) {
      gids[index] = spike->gid;
      simulation_times[index] = spike->simulation_time;
    }
  }

  response.set_body(web::json::value::object(
      {{"simulation_times", simulation_times}, {"gids", gids}}));
  return response;
}

web::http::http_response HttpServer::GetMultimeterMeasurement(
    const web::http::http_request& request) {
  web::http::http_response response(web::http::status_codes::OK);
  web::json::value body = web::json::value::object();
  
  const auto parameters = web::uri::split_query(request.request_uri().query());
  const auto parameter_multimeter_id = parameters.find("multimeter_id");
  const auto parameter_attribute = parameters.find("attribute");
  const auto parameter_from = parameters.find("from");
  const auto parameter_to = parameters.find("to");
  const auto parameter_gids = parameters.find("gids");
  const auto parameter_offset = parameters.find("offset");
  const auto parameter_limit = parameters.find("limit");

  const auto multimeter_id = std::stoll(parameter_multimeter_id->second);
  const auto attribute = parameter_attribute->second;
  const auto filter_gids = parameter_gids != parameters.end() 
    ? std::vector<std::uint64_t>(
        parameter_gids->second.begin(), 
        parameter_gids->second.end()) 
    : std::vector<std::uint64_t>();

  const auto measurements = storage_->GetMultimeterMeasurements();
  if (measurements.find(multimeter_id) != measurements.end() &&
      measurements.at(multimeter_id).find(attribute) 
      != measurements.at(multimeter_id).end()) {
    auto& measurement = measurements.at(multimeter_id).at(attribute);
    auto& simulation_times = measurement.simulation_times;
    auto& gids = measurement.gids;
    auto& values = measurement.values;

    auto simulation_times_begin = parameter_from == parameters.end() ? simulation_times.begin() 
      : std::lower_bound(simulation_times.begin(), simulation_times.end(), std::stoll(parameter_from->second));
    auto simulation_times_end = parameter_to == parameters.end() ? simulation_times.end() 
      : std::lower_bound(simulation_times.begin(), simulation_times.end(), std::stoll(parameter_to->second));
    auto simulation_times_subset = std::vector<web::json::value>(
      simulation_times_begin, simulation_times_end);
    std::size_t simulation_start_index = std::distance(simulation_times.begin(), simulation_times_begin);
    std::size_t simulation_end_index = std::distance(simulation_times.begin(), simulation_times_end);
    // TODO: Cull subset, start and end index by offset/limit.

    auto gids_begin = filter_gids.empty() ? gids.begin() : filter_gids.begin();
    auto gids_end = filter_gids.empty() ? gids.end() : filter_gids.end(); 
    auto gids_subset = std::vector<web::json::value>(gids_begin, gids_end);
    auto gid_indices = std::vector<std::size_t>();
    if (!filter_gids.empty())
      for (auto& filter_gid : filter_gids)
        gid_indices.push_back(std::distance(gids.begin(), std::find(gids.begin(), gids.end(), filter_gid)));
    else {
      gid_indices.resize(gids.size());
      std::iota(gid_indices.begin(), gid_indices.end(), 0);
    }
    
    auto values_subset = std::vector<web::json::value>(simulation_times_subset.size() * gids_subset.size());
    for (std::size_t t = 0, vt = simulation_start_index; vt < simulation_end_index; ++t, ++vt)
      for (std::size_t g = 0; g < gid_indices.size(); ++g)
        values_subset[t * gids_subset.size() + g] = values[vt * gids.size() + gid_indices[g]];

    body["simulation_times"] = web::json::value::array(simulation_times_subset);
    body["gids"] = web::json::value::array(gids_subset);
    body["values"] = web::json::value::array(values_subset);
  }

  response.set_body(body);
  return response;
}
}  // namespace insite
