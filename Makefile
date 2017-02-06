all: sample lib_static

# Define compile and link flags
-include config.local
CXX ?= g++
#opt = -g3 -O0
opt = -g0 -O3
cf = $(opt) -Wall -Wextra -Wuninitialized --std=c++11 -I./include/result_table -fPIC
lf = $(opt) --std=c++11

# Object director
objDir:= build/
build_dir = @mkdir -p $(dir $@)

# Get source file names
src_files := $(patsubst src/%,build/src/%, $(patsubst %.cpp,%.o,$(wildcard src/*.cpp)))

# Build sample
sample: $(src_files) $(objDir)sample.o
	$(CXX) -o sample $(objDir)sample.o $(src_files) $(lf)

# Build as library
lib_static: $(src_files)
	ar rvs libresulttable.a $(src_files)

# Build as library
lib_shared: $(src_files)
	$(CXX) -shared -o libresulttable.so $(src_files) $(lf)

# Command for building and keeping track of changed files 
$(objDir)%.o: %.cpp
	$(build_dir)
	$(CXX) -MD -c -o $@ $< $(cf)
	@cp $(objDir)$*.d $(objDir)$*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $(objDir)$*.d >> $(objDir)$*.P; \
		rm -f $(objDir)$*.d

-include $(objDir)*.P

# Clean up =)
clean:
	find -name sample -delete
	find -name libresulttable.so -delete
	find -name libresulttable.a -delete
	rm build -rf
