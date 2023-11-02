#!/bin/bash

exit_with_message() {
  echo "$1"
  echo "Press Enter to continue"
  read -n 1
  exit "${2:-1}"
}
