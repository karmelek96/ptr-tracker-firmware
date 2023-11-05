### MOVED
https://github.com/PTR-projects/PTR_tracker_firmware

## Building the firmware
# Building using Docker
1. Install [Docker](https://www.docker.com)
2. Clone the repository
3. Set the working directory to the repository root
4. Build the Docker image:
```
docker build -t tracker_compiler .
```

5. Build the firmware:
```
docker run --rm -it -v $(pwd):/usr/local/src/tracker_firmware tracker_compiler
```
6. The firmware will be in the `build` directory

# Uploading on windows using openocd and stlink-v2
7. Run openocd:
```
openocd_windows/bin/openocd.exe -s openocd_windows/share/openocd/scripts -f interface/stlink.cfg -f target/stm32g0x.cfg -c "program build/PTR-tracker-C.hex verify reset exit"
```
