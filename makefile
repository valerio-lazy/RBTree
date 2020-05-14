SrcFiles=$(wildcard *.cpp)
ObjFiles=$(patsubst %.cpp, %.o, $(SrcFiles))
AppFile=a.out

all:$(AppFile)


$(AppFile):$(SrcFiles) 
	g++ $^ -o $@ -std=c++11 -Wall -fno-elide-constructors

clean:
	rm -f $(AppFile) $(ObjFiles)
