## Top level makefile to build the CMake targets

PROJECT=project_name

CMAKE_ARGS= -DCMAKE_TOOLCHAIN_FILE=../cmake/gcc_stm32.cmake \
			-DSTM32_CHIP=STM32F103CB \
			-DCMAKE_PROJECT_NAME=$(PROJECT)

default: build
	@$(MAKE) -C build
	@$(MAKE) -C build $(PROJECT).hex

build:
	@mkdir -p $@
	@cd $@ && cmake $(CMAKE_ARGS) ../

clean:
	rm -rf build

flash:
	st-flash --format ihex write build/$(PROJECT).hex

.phony: build clean flash
