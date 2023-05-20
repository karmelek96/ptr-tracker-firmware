### PTR Tracker firmware

## Building the firmware
# Building using Docker
1. Install [docker](https://www.docker.com)
2. Clone the repository
3. Set the working directory to the repository root
4. Build the Docker image:
```docker build -t tracker_compiler .```

5. Build the firmware:
```docker run --rm -it -v $(pwd):/usr/local/src/tracker_firmware tracker_compiler```

6. The firmware will be in the `build` directory