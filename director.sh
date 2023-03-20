#!/bin/bash

# First Name: Aditya Kamleshkumar
# Last Name: Patel
# Student Number: 501122872
# Email: aditya.patel@torontomu.ca

# Check for invalid command line arguments
if [ $# -ne 2 ]; then
  echo "Required Input: $0 W sPID"
  exit 1
fi

# Arguments entered
W=$1
sPID=$2

# Define a function to display the contents of the worker files
display() {
  clear
  for ((i=1; i<=W; i++)); do
    echo "Worker Process $i:"
    cat "worker_$i.txt"
    echo
  done
}

# Deleting all the worker_*.txt and worker_*.pid files before terminating
cleanup() {
  #Making sure all the worker processes are killed
  for ((i=1; i<=W; i++)); do
    kill $(cat "worker_$i.pid") 2>/dev/null
  done

  # worker_*.txt files
  for ((i=1; i<=W; i++)); do
    rm "worker_$i.txt"
  done

  #worker_*.pid files
  for ((i=1;i<=W;i++)); do
    rm "worker_$i.pid"
  done

  echo
  echo "-> worker processes terminated.";
  echo "-> worker files deleted."
  echo "-> director terminated."
  echo
  exit 0
}

# Trap the SIGINT signal to perform clean-up before terminating
trap cleanup SIGINT

# Initiating worker processes
for ((i=1; i<=W; i++)); do

  # Define the worker file names
  worker_file="worker_$i.txt"
  pid_file="worker_$i.pid"

  # Run the worker process in background and record its PID
  ./worker "worker_$i.txt" $2 &
  echo $! > "$pid_file"
done

# Loop forever, calling the display function once per second
while true; do
	display
	sleep 1
done