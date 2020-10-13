import pytest
import subprocess
import signal
import requests
import time
import os

@pytest.fixture(scope="session")
def nest_simulation(request):
    logfile = open("docker-compose.log", "w")
    process = subprocess.Popen(["docker-compose", "up", "--build"], stdout=logfile, stderr=subprocess.STDOUT)

    def finalize():
        # process.send_signal(signal.SIGINT)
        os.kill(process.pid, signal.SIGINT)
        try:
            process.wait(10)
        except subprocess.TimeoutExpired:
            print("Sending SIGKILL to docker-compose")
            os.kill(process.pid, signal.SIGKILL)
            process.wait()
        compose_down_process = subprocess.Popen(["docker-compose", "down"], stdout=logfile, stderr=subprocess.STDOUT)
        compose_down_process.wait()
        logfile.close()
    request.addfinalizer(finalize)

    begin_time = time.time()
    while True:
        time.sleep(1.0)
        try:
            r = requests.get("http://localhost:8080/v1/nest/simulationTimeInfo")
            if r.status_code == 200:
                break
        except requests.exceptions.ConnectionError:
            pass
        current_time = time.time()
        if current_time - begin_time > 20:
            pytest.fail('Timeout during container start')

    return process