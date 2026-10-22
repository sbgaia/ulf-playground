EXAMPLE ?= tutorial/InterruptModal
NAME := $(notdir $(EXAMPLE))
BUILD := build/$(NAME)

.PHONY: build run clean

build:
	cmake -B $(BUILD) -DLF_MAIN=$(EXAMPLE)
	cmake --build $(BUILD)

run: build
	./$(BUILD)/$(NAME)

clean:
	rm -rf build
