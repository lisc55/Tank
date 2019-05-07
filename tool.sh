if [ "$1" = "make" ]; then
	if [ ! -d "build" ]; then
		mkdir build
	fi
	cd build && cmake ../ && make && cd ..
elif [ "$1" = "clean" -a -d "/build" ] ; then
	rm Tank && cd build && rm -rf * && cd ..
elif [ "$1" = "merge" ]; then
	python3 merge.py
elif [ "$1" = "format" ]; then
	clang-format -style=file -i `find . -type f -regex ".*\.\(cpp\|h\)"`
else
	echo "ERROR"
fi
echo "FINISHED"