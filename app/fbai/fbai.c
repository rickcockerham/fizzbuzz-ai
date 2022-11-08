//################################################################################
// Fbai class.
// A C program that learns the Fizz Buzz problem.
// I used my neural network code that I downloaded years ago and modified.
// I have simplified the neural network code to make it only has complicated as
// it needs to be to solve this simple problem.  One hidden layer and four separate
// networks is all it takes to solve the FB problem in 5-10 learning runs.
//################################################################################

#include <stdio.h>
#include <stdlib.h> 
#include <ruby.h>

//nn constants
double eta = 0.03; // a very slow learning rate
double alpha = 0.90, smallwt = 0.5;
#define PASSES 1 //passes per training run.
#define NUMPATTERNS 100 // 1-100
#define NUMHIDDEN 1 // hidden layers
#define NUMINPUTS 1 //number of inputs. In this case it's just a single integer.
#define NNS 4 // one neural network output per norm,fizz,buzz,fizzbuzz
double expected[NNS][NUMPATTERNS+1]; //holds the test data.

// The weights and outputs of the neural network.
double WeightIH[NNS][NUMINPUTS+1][NUMHIDDEN+1];//2. bias and input
double WeightHO[NNS][NUMHIDDEN+1];
double SumH[NNS][NUMPATTERNS+1][NUMHIDDEN+1], Hidden[NNS][NUMPATTERNS+1][NUMHIDDEN+1];
double SumO[NNS][NUMPATTERNS+1], Output[NNS][NUMPATTERNS+1];
double DeltaO[NNS], SumDOW[NNS][NUMHIDDEN+1], DeltaH[NNS][NUMHIDDEN+1];
double DeltaWeightIH[NNS][NUMINPUTS+1][NUMHIDDEN+1], DeltaWeightHO[NNS][NUMHIDDEN+1];

// The names of each network.
#define NORM 0
#define FIZZ 1
#define BUZZ 2
#define FIZZBUZZ 3

void train();
int gotest(int);
void init_ai();

//########################################
//random double.  I would normally use lib sodium for better random. 
double randd() {
  return rand() / (double)RAND_MAX;
}

//################################################################################
// Ruby - C interface code.
// These functions are exposed to Ruby.

VALUE rb_fbai;

VALUE gotrain(VALUE self) {
  train();//run a single training epoch.
  return self;
}

VALUE reinit(VALUE self) {
  init_ai();// clear the weights and start over.
  return self;
}

VALUE valueat(VALUE self, VALUE testnum) {
  int testx = NUM2INT(testnum); //convert VALUE to int

  int returnval = gotest(testx);// get the AI output for this int.
  
  return INT2NUM(returnval);//convert it back to a VALUE
}

// define interface methods between Ruby and C
void Init_fbai() {
  rb_fbai = rb_define_class("Fbai", rb_cObject);
  rb_define_method(rb_fbai, "gotrain", gotrain, 0);
  rb_define_method(rb_fbai, "valueat", valueat, 1);
  rb_define_method(rb_fbai, "reinit", reinit, 0);

  init_ai();
}

//################################################################################
// the patterns are the training data.  It's an array of 3 values 0 if it's just x, 1 if it's %3, 2 if it's %5, and 3 if it's %3&%5
// these are stored in three values for three sets of training patterns.

void patterns() {

  memset(expected, 0.0, NNS * NUMPATTERNS+1); // all zeros first

  for(int x = 1; x <= 100; x++) {
    
    if(x % 3 == 0 && x % 5 == 0) {
      expected[FIZZBUZZ][x] = 1.0; // set expexted[3][x] = 1.0 the rest are zeros.
    } else if(x % 5 == 0) {
      expected[BUZZ][x] = 1.0;
    } else if(x % 3 == 0) {
      expected[FIZZ][x] = 1.0;
    } else {
      expected[NORM][x] = 1.0;
    }
  }
}

//################################################################################
// clear all the weights and randomize them to learn the problem or learn it again.
void init_ai() {
  int nn,hid,i;

  ////////////////////////////////////////////////////////////////////////////////
  // init nn

  memset(Output, 0.0, NNS * (NUMPATTERNS+1)); // all zeros for outputs.
  memset(SumH, 0.0, NNS * (NUMPATTERNS+1) * (NUMHIDDEN+1));
  memset(Hidden, 0.0, NNS * (NUMPATTERNS+1) * (NUMHIDDEN+1));
  memset(SumO, 0.0, NNS * (NUMPATTERNS+1));
  memset(SumDOW, 0.0, NNS * (NUMHIDDEN+1));
  memset(DeltaH, 0.0, NNS * (NUMHIDDEN+1));
  memset(DeltaWeightIH, 0.0, NNS * (NUMINPUTS+1) * (NUMHIDDEN+1));
  memset(DeltaWeightHO, 0.0, NNS * (NUMHIDDEN+1));

  for(nn = 0; nn < NNS; nn++) { // loop through all neural nets

    for( hid = 0 ; hid < NUMHIDDEN ; hid++ ) {    /* initialize WeightIH and DeltaWeightIH */
      for( i = 0; i <= 1; i++) {
	DeltaWeightIH[nn][i][hid] = 0.0 ;
	WeightIH[nn][i][hid] = 2.0 * ( randd() - 0.5 ) * smallwt ;
      }
    }
    /* initialize WeightHO and DeltaWeightHO */
    for( hid = 0 ; hid < NUMHIDDEN ; hid++ ) {
      DeltaWeightHO[nn][hid] = 0.0 ;
      WeightHO[nn][hid] = 2.0 * ( randd() - 0.5 ) * smallwt ;
    }
    
  }
  patterns();
}

//#######################################
// This is basically my test.  It the answer is correct it will return 0-3 otherwise it returns -1.
int gotest(int x) {
  double max = 0.0;
  int nn, maxx = 0;
  int correct = 0;
  for(nn = 0; nn < NNS; nn++) {
    if(Output[nn][x] > max) {
      max = Output[nn][x];
      maxx = nn;
      if(1.0 == expected[nn][x]) correct = 1;
    }
  }
  return (1 == correct ? maxx : -1);
}


//################################################################################
//################################################################################
//################################################################################
// training run for the AI.
// This code was downloaded from the internet many years ago and modifed for my other AI project ai-stocktrading.com
// I've simplifed it for this small demo.
void train() {

  int hid, nn, p, epoch;
  
  for(nn = 0; nn < NNS; nn++) { // loop through the neural nets

    for( epoch = 0 ; epoch < PASSES ; epoch++) {
      
      for( p = 1 ; p < NUMPATTERNS+1 ; p++ ) {    /* repeat for all the training patterns */

	for( hid = 1 ; hid <= NUMHIDDEN ; hid++ ) {    /* compute hidden unit activations */
	  SumH[nn][p][hid] = WeightIH[nn][0][hid] ;// bias against input 0
	  
	  SumH[nn][p][hid] += expected[nn][p] * WeightIH[nn][1][hid]; //double np is the input 1-100
	  Hidden[nn][p][hid] = 1.0/(1.0 + exp(-SumH[nn][p][hid])) ;
	}
	

	/* compute output unit activations and errors */
	SumO[nn][p] = WeightHO[nn][0]; // bias
	for( hid = 1 ; hid <= NUMHIDDEN ; hid++ ) {
	  SumO[nn][p] += Hidden[nn][p][hid] * WeightHO[nn][hid] ;
	}
	
	// Sigmoidal Outputs
	Output[nn][p] = 1.0/(1.0 + exp(-SumO[nn][p]));

	//Sigmoidal Outputs, SSE
	DeltaO[nn] = (expected[nn][p] - Output[nn][p]) * Output[nn][p] * (1.0 - Output[nn][p]) ;

	for( hid = 1 ; hid <= NUMHIDDEN ; hid++ ) {    /* 'back-propagate' errors to hidden layer */
	  SumDOW[nn][hid] = WeightHO[nn][hid] * DeltaO[nn];
	  DeltaH[nn][hid] = SumDOW[nn][hid] * Hidden[nn][p][hid] * (1.0 - Hidden[nn][p][hid]) ;
	}

	for( hid = 1; hid <= NUMHIDDEN ; hid++ ) {     /* update weights WeightIH */
	  DeltaWeightIH[nn][0][hid] = eta * DeltaH[nn][hid] + alpha * DeltaWeightIH[nn][0][hid] ;
	  WeightIH[nn][0][hid] += DeltaWeightIH[nn][0][hid] ;

	  DeltaWeightIH[nn][1][hid] = eta * expected[nn][p] * DeltaH[nn][hid] + alpha * DeltaWeightIH[nn][1][hid];
	  WeightIH[nn][1][hid] += DeltaWeightIH[nn][1][hid] ;
	}

	/* update weights WeightHO */
	DeltaWeightHO[nn][0] = eta * DeltaO[nn] + alpha * DeltaWeightHO[nn][0] ;
	WeightHO[nn][0] += DeltaWeightHO[nn][0] ;
	for( hid = 1 ; hid <= NUMHIDDEN ; hid++ ) {
	  DeltaWeightHO[nn][hid] = eta * Hidden[nn][p][hid] * DeltaO[nn] + alpha * DeltaWeightHO[nn][hid] ;
	  WeightHO[nn][hid] += DeltaWeightHO[nn][hid] ;
	}

      }//for num patterns

    } // for epochs
      
  }//for nns

}// train

