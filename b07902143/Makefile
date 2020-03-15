source = hw1_5 hw1_6
all: $(source)
$(source):
%: %.c
	g++ -O2 -std=c11 $< -o $@
%: %.cpp
	g++ -O2 -std=c++14 $< -o $@
clean:
	rm -rf $(source)
run5:
	./hw1_5 < input
run6:
	./hw1_6 < input
