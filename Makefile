check:
	make format
	pio run
sim:
	g++ test/test_sim/API.cpp test/test_sim/simulator.cpp src/maze.cpp src/virtual_mouse.cpp src/solver.cpp src/util.cpp src/mouse.cpp -I include/ -o out/sim.out

clean:
	rm -rf out docs

format:
	@find . -regex '.*\.\(cpp\|hpp\|cu\|cuh\|c\|h\)' -exec clang-format -style=file -i {} \;

docs:
	git submodule update doxygen-awesome-css
	doxygen Doxyfile
	xdg-open docs/html/index.html

.PHONY: docs format
