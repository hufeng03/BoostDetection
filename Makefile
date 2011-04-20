CXX = g++
CPPFLAGS = `pkg-config --cflags opencv`
LDFLAGS = `pkg-config --libs opencv` 
LIBS = liblinear-weights-1.51/blas/blas.a

all: train test show

train: train.o Detector.o CascadeClassifier.o BoostClassifier.o CartClassifier.o SvmClassifier.o liblinear-weights-1.51/linear.o liblinear-weights-1.51/tron.o WeakClassifier.o Classifier.o Features.o HaarFeatures.o EdgeletFeatures.o HogFeatures.o DataSet.o
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS)

test: test.o Detector.o CascadeClassifier.o BoostClassifier.o CartClassifier.o SvmClassifier.o liblinear-weights-1.51/linear.o liblinear-weights-1.51/tron.o WeakClassifier.o Classifier.o Features.o HaarFeatures.o EdgeletFeatures.o HogFeatures.o DataSet.o
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS)

show: show.o Detector.o CascadeClassifier.o BoostClassifier.o CartClassifier.o SvmClassifier.o liblinear-weights-1.51/linear.o liblinear-weights-1.51/tron.o WeakClassifier.o Classifier.o Features.o HaarFeatures.o EdgeletFeatures.o HogFeatures.o DataSet.o
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS)


clean:
	rm -f train test *.o 
