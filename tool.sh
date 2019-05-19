if [ "$1" = "make" ]; then
	if [ ! -d "build" ]; then
		mkdir build
	fi
	cd build && cmake ../ && make && cd ..
elif [ "$1" = "clean" ] ; then
	rm Tank && cd build && rm -rf * && cd .. && rm -rf Tank.dSYM
elif [ "$1" = "merge" ]; then
	python3 merge.py
elif [ "$1" = "format" ]; then
	clang-format -style=file -i json/*.h json/*.cpp src/*.cpp src/*.h *.cpp Amazons/*.cpp
else
	echo "NO SUCH CMD"
fi
echo "FINISHED"
