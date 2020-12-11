# Parallel Monte Carlo for Radiative Transfer Using DPC++ 
This is a repository for CSCI 596 final project. The main topic is build a parallel monte carlo solver for radiative transfer problem using DPC++ language.

## Radiative transfer
Knowing the medium property and energy source, decribe the energy in the medium.

![image](https://github.com/yzhu0447/cs596_project/blob/main/IMG/1.png)

## Radiative transfer equation
The Boltzmann equation for radiative transfer

![image](https://github.com/yzhu0447/cs596_project/blob/main/IMG/5.png)

## Monte Carlo method
Treat the source as a independent set of photons each carrying part of the total energy 

![image](https://github.com/yzhu0447/cs596_project/blob/main/IMG/3.png)

The life of a photon starts with emitting from source and ends with totally absorption

![image](https://github.com/yzhu0447/cs596_project/blob/main/IMG/2.png)

## Performance tuning and load balancing

![image](https://github.com/yzhu0447/cs596_project/blob/main/IMG/6.png)
![image](https://github.com/yzhu0447/cs596_project/blob/main/IMG/7.png)
