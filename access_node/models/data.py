# coding: utf-8

from __future__ import absolute_import
from datetime import date, datetime  # noqa: F401

from typing import List, Dict  # noqa: F401

from access_node.models.base_model_ import Model
from access_node import util


class Data(Model):
    """NOTE: This class is auto generated by the swagger code generator program.

    Do not edit the class manually.
    """

    def __init__(self, simulation_steps: List[float]=None, neuron_ids: List[float]=None, values: List[float]=None):  # noqa: E501
        """Data - a model defined in Swagger

        :param simulation_steps: The simulation_steps of this Data.  # noqa: E501
        :type simulation_steps: List[float]
        :param neuron_ids: The neuron_ids of this Data.  # noqa: E501
        :type neuron_ids: List[float]
        :param values: The values of this Data.  # noqa: E501
        :type values: List[float]
        """
        self.swagger_types = {
            'simulation_steps': List[float],
            'neuron_ids': List[float],
            'values': List[float]
        }

        self.attribute_map = {
            'simulation_steps': 'simulation_steps',
            'neuron_ids': 'neuron_ids',
            'values': 'values'
        }

        self._simulation_steps = simulation_steps
        self._neuron_ids = neuron_ids
        self._values = values

    @classmethod
    def from_dict(cls, dikt) -> 'Data':
        """Returns the dict as a model

        :param dikt: A dict.
        :type: dict
        :return: The Data of this Data.  # noqa: E501
        :rtype: Data
        """
        return util.deserialize_model(dikt, cls)

    @property
    def simulation_steps(self) -> List[float]:
        """Gets the simulation_steps of this Data.


        :return: The simulation_steps of this Data.
        :rtype: List[float]
        """
        return self._simulation_steps

    @simulation_steps.setter
    def simulation_steps(self, simulation_steps: List[float]):
        """Sets the simulation_steps of this Data.


        :param simulation_steps: The simulation_steps of this Data.
        :type simulation_steps: List[float]
        """

        self._simulation_steps = simulation_steps

    @property
    def neuron_ids(self) -> List[float]:
        """Gets the neuron_ids of this Data.


        :return: The neuron_ids of this Data.
        :rtype: List[float]
        """
        return self._neuron_ids

    @neuron_ids.setter
    def neuron_ids(self, neuron_ids: List[float]):
        """Sets the neuron_ids of this Data.


        :param neuron_ids: The neuron_ids of this Data.
        :type neuron_ids: List[float]
        """

        self._neuron_ids = neuron_ids

    @property
    def values(self) -> List[float]:
        """Gets the values of this Data.


        :return: The values of this Data.
        :rtype: List[float]
        """
        return self._values

    @values.setter
    def values(self, values: List[float]):
        """Sets the values of this Data.


        :param values: The values of this Data.
        :type values: List[float]
        """

        self._values = values
