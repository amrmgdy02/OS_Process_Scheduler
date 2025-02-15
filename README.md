# Operating System Scheduler

A simulation of a process scheduler using C and IPC techniques. This project implements the Shortest Remaining Time Next (SRTN) and Highest Priority First (HPF) scheduling algorithms and utilizes a buddy system for memory allocation. Inter-Process Communication (IPC) between the scheduler and the processes is achieved using message queues and signals.

## Overview

The Operating System Scheduler project simulates a process scheduler in a multi-process environment. It is designed to demonstrate the following:
- Process scheduling using SRTN and HPF algorithms.
- Memory management using the buddy system allocation technique.
- IPC using message queues and signals.

This project is an educational tool for understanding how process scheduling and memory management work at a lower level in operating systems.

## Features

- **SRTN Scheduling:** Implements the Shortest Remaining Time Next algorithm for preemptive scheduling.
- **HPF Scheduling:** Implements the Highest Priority First algorithm.
- **Buddy System Memory Allocation:** Uses the buddy system to manage memory allocation efficiently.
- **IPC Mechanisms:** Uses message queues and signals to handle communication between the scheduler and processes.
- **Simulation Environment:** A controlled simulation of process scheduling to demonstrate algorithm behavior and resource management.

## Architecture

The project is structured as follows:

- **Scheduler Module:** Contains the main scheduling logic for both SRTN and HPF algorithms.
- **Process Module:** Simulates processes that are scheduled by the scheduler.
- **Memory Manager:** Implements the buddy system for dynamic memory allocation.
- **IPC Manager:** Manages inter-process communication using message queues and signals.
- **Utilities:** Common functions and data structures used across modules.
