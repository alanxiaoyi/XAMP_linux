########################################################################
	ACAPP: Analytical Cache Performance Prediction tool suite 
	version: 1.0.0
		
	Author: Shaunak Joshi, Fei Guo and Yan Solihin
	Date: 01/15/2007	

########################################################################


OVERVIEW
---------------------
This README file is for the acapp tool suite 1.0.0.  This release
is available from:
        http://www.ece.ncsu.edu/arpers/index_files/Page516.htm
in the file acapp1.0.0.tar.gz. 


INTRODUCTION
---------------------

The acapp Tool Suite is a set of tools designed to accurately predict the cache 
miss rates of single application for different cache parameters including cache 
associativity and cache replacement policy as well as the miss rates of two
applications when they share the same level cache.  The acapp Tool Suite uses 
circular sequence profile(s)[2] as inputs for all prediction options.  Therefore, 
the suite also contains a cache profile generation tool, which has been incorporated
into SimpleScalar. The tool also incorporates the functionality of being able to 
create user-specified replacement policies for prediction. A logfile (rplist.log) 
maintains a list of all supported replacement policies.


INSTALLATION
---------------------

To install the acapp tool, first unzip the package
	tar -xvzf acapp.1.0.0 
The following files will be shown in the package directory :
- acapp.c
- changeAssoc.c
- common.c
- contention.c
- changerp.c
- createrp.c
- acapp.h
- changeAssoc.h
- common.h
- contention.h
- changerp.h
- createrp.h
- Makefile
- README.txt
- License.txt
- rplist.log
- usr_rp.in
- ./csq/benchmark1.csq
- ./csq/benchmark2.csq
- ./csq/benchmark3.csq
- ./fine/.
- ./addOnSimpleScalar/acappProfiler.c
- ./addOnSimpleScalar/cache.c
- ./addOnSimpleScalar/sim-outorder.c
- ./addOnSimpleScalar/acappProfiler.h
- ./addOnSimpleScalar/main.c
- ./addOnSimpleScalar/Makefile

and simply type
	make
an executable file ./acapp is created.


SOURCE FILES TREE
-------------------------------  

The source files of the tool suite are listed below:
- acapp.c
- changeAssoc.c
- common.c
- contention.c 
- changerp.c
- createrp.c

Source files that are used for SIMPLESCALAR extension:
- cache.c
- acappProfiler.c
- sim-outorder.c
- main.c

The brief description for each source file is listed below:

ACAPP.C
	This contains the main() function of the tool source code.  It provides code
	that serves as a command line interface.  However, its primary purpose is to 
	call functions for corresponding prediction option according to the 
	user-specified varying cache parameter, as well as allocating and deallocating
	memory space for prediction purpose. acapp.C calls functions from the source 
	files listed below.

CHANGEASSOC.C
	Contains code that calculates the miss rate for a particular associativity 
	(or range of associativities) and compares it to the original miss rate 
	calculated from the input profile.
	
COMMONC.C
	Contains the source code that initializes data structures containing the 
	stack distance and other cache parameters read from the input profile for 
	each application. Other data structures required for prediction are also 
	initialized. It also contains a function that determines the replacement 
	probability function of the user specified replacement policy according to 
	the usr_rp.in.  

CONTENTION.C
	This source code file is based on an algorithm [2] that predicts the miss 
	rate of two applications under cache sharing.  This miss rate is then 
	compared to the original miss rate of each application (without cache 
	sharing).

CHANGERP.C
	This source code file is based on an algorithm that predicts the miss rate 
	under different different cache replacement policies. The default supported 
	replacement polices are: NMRU4, NMRU1, MRUskw, LRUskw[1].User is able to 
	create any other user-created replacement policies. The user specifies an 
	index number (in rp_list.log) that represents the replacement policy and a 
	miss rate will be predicted according to the input profile by using an 
	algorithm [1] that gathers the coefficient information from a directory
	(./fine).

CREATERP.C
	This source code file is based on an algorithm [1] that creates coefficient 
	files that are used to predict the miss rate for a user-created replacement 
	policy.  The coefficients are stored in the ./fine directory

	
/ADDONSIMPLESCALAR/CACHE.C
	This is a modified SimpleScalar source file.  It has been modified to allow 
	for generation of circular sequence profiles by sending L2 cache accesses to
	acappProfiler.c from SimpleScalar.It should replace the original SimpleScalar
	cache.c source file in the simplesim-3.0 directory. All the modifications 
	have been commented by the line:
		/********** Modification for acapp tool **********/
	Modified code is found mainly in the function cache_access()

/ADDONSIMPLESCALAR/ACAPPPROFILER.C
	This file contains the code that generates the circular sequence profile. It
	receives L2 cache accesses from SimpleScalar through cache.c.  This file 
	should also be placed in the simplesim-3.0 working directory.

/ADDONSIMPLESCALAR/SIM-OUTORDER.C
	This is a modified SimpleScalar source file.  It has been modified to allow 
	for the creation of circular sequence profiles in a file format (or .CSQ file).
	The .CSQ file is created in the name of the benchmark that is run.  This file
	should replace the original SimpleScalar sim-outorder.c file in the
	simplesim-3.0 directory.  All the modifications have been commented by 
	the line:
		/********** Modification for acapp tool **********/
	Modified code is found mainly in the function sim_load_prog()

/ADDONSIMPLESCALAR/MAIN.C
	This is a modified SimpleScalar source file.  It has been modified to allow
	to call the printCacheStats() fucntion from acappProfiler.c to create
	a .CSQ file.


/ADDONSIMPLESCALAR/Makefile
	The SimpleScalar Makefile has been included as part of the package.  It has 
	been modified to include
	acappProfiler.c in the complication of the source code and header files.
	acappProfiler.c should be added to the list of SRC files. 
	acappProfiler.h should be added to the list of HDR files
	acappProfiler.o should be added to the list of OBJS
	This Makefile should replace the original SimpleScalar Makefile in the 
	simplesim-3.0 directory.

PROFILE (.CSQ) FILE GENERATION
------------------------------

The acapp tool has added functionality that allows SimpleScalar to generate circular 
sequence profiles, or in other words .csq files. These .csq files are used as inputs
to the tool.Once the simplescalar addon files are copied into simplescalar home 
directory , users need to recompile simplescalar to make profile generation tool 
work. Then when users typically type the 
./sim-outorder <benchmark> execution command for regular simulation. 
The .csq file will be created with the default name of <benchmark>.csq and 
saved in the simplescalar working directory.

EXAMPLE
	% ./sim-outorder gcc.train.eio

	The file produced will be gcc.train.eio.csq.

Please refer to the SimpleScalar manual for make and usage details of sim-outorder.
	
Note: Any simulator can be modified to generate the circular sequence profile. If 
      user try to make his own code to generate the circular sequence profile, please
      ensure that the CSQ file is created with following format. It should contain 
      the stack distance and weighted average values of 4 times the real cache 
      associativity. Please refer to [1] for more information.
      
      The .csq file should be in the following format:
      
      #sets <integer power of 2>
      #assoc <integer power of 2>
      #scaling_factor 4
      #block_size <integer power of 2>

      #cseq
      <list of values separated by spaces>

      #stackDist
      <list of values separated by spaces>
      
      
ACAPP TOOL USAGE
--------------------

The acapp tool help menu can be displayed using
%acapp -h

******** acapp TOOL HELP MENU ********
General Usage:
-h  --- HELP MENU
Prediction under varying cache associativity:
         -a <assoc> [<min assoc) <max assoc>] -f1 <profile1>
Prediction under varying cache replacement policies:
         -p <rpindex> -f1 <profile1>
         -pA -f1 <profile1>
Prediction under cache sharing:
         -c -f1 <profile1> -f2 <profile2>
Adding new replacement policy:(require 'usr_rp.in')
         -n (default) or
         -n <dx> <nxmin> <nxmax>
Print supported replacement policies
         -log


USAGE EXAMPLES:
---------------------------

Note: benchmark1.csq represents the swim benchmark with ref input set.
      benchmark2.csq represents the apsi benchmark with ref input set.
      benchmark3.csq represents the aamp benchmark with ref input set.
	
Prediction under cache sharing:
	acapp -c -f1 <profile1> -f2 <profile2>

	EXAMPLE
		./acapp -c -f1 ./csq/benchmark1.csq -f2 ./csq/benchmark2.csq

	EXPECTED OUTPUT

	The CSQ file is generated using the following cache parameters:
        	Sets: 1024
	        Associativity: 4
	        Block size: 64
                        ******** RESULTS ********

                         ./csq/benchmark1.csq    ./csq/benchmark2.csq
	Accesses:                68182266                        11805186
	Predicted miss rate:     0.868560                        0.338920
	Original miss rate:      0.768043                        0.244366


Prediction under varying cache associativity
	acapp -a <assoc> [<min assoc> <max assoc>] -f1 <profile1>
	
	EXAMPLE
		./acapp -a 4 7 -f1 ./csq/benchmark1.csq
		
	EXPECTED OUTPUT
	The CSQ file is generated using the following cache parameters:
        	Sets: 1024
	        Associativity: 4
        	Block size: 64
	Original miss rate: 0.768043
	Miss rate for A = 4:  0.768043
	Miss rate for A = 5:  0.733912
	Miss rate for A = 6:  0.689150
	Miss rate for A = 7:  0.607186



Print supported replacement policies
	-log
		Prints the supported replacement policies.
	EXAMPLE
		./acapp -log
	
	EXPECTED OUTPUT (default)
		* * * * SUPPORTED REPLACEMENT POLICIES LOGFILE * * * *
		1 - NMRU4
		2 - NMRU1
		3 - LRUskw
		4 - MRUskw

	Note: The explanation of each of these replacement policies can be 
	found in [1].


Prediction under varying cache replacement policies
	acapp -p <rpindex> -f1 <profile1>
	
	EXAMPLE 
		./acapp -p 2 -f1 ./csq/benchmark1.csq
	
	EXPECTED OUTPUT
	The CSQ file is generated using the following cache parameters:
	        Sets: 1024
        	Associativity: 4
	        Block size: 64
	Prediction Result for NMRU1
	LRU: 0.768043
	Pred: 0.739310

	

	acapp -pA -f1 <profile1>
		predict the miss rate for all supported replacement policies
	
	EXAMPLE 
		./acapp -pA -f1 ./csq/benchmark1.csq	
	
	EXPECTED OUTPUT
	The CSQ file is generated using the following cache parameters:
        Sets: 1024
        Associativity: 4
        Block size: 64

	1 - NMRU4
	Missing Coefficient file: ./fine/NMRU4/d_0/NMRU4:4_0_0

	2 - NMRU1
	Prediction Result for NMRU1
	LRU: 0.768043
	Pred: 0.739310
	*******************************

	3 - LRUskw
	Prediction Result for LRUskw
	LRU: 0.768043
	Pred: 0.742833
	*******************************

	4 - MRUskw
	Prediction Result for MRUskw
	LRU: 0.768043
	Pred: 0.740608
	*******************************

	Note: 1-NMRU4 has an error because prediction for the NMRU4 replacement 
	policy only works when associativity > 4



Adding new replacement policy or adding required coefficient files:
	(require 'usr_rp.in')
	acapp -n (default)
	
		Produces coefficient files for the user-defined replacement policy 
		defined in usr_rp.in. The default setting creates coefficient files 
		of d with 0 to 3 times that of the original associativity[1] and a 
		range of average n from 0 to 20.
	
	acapp -n <dx> <nxmin> <nxmax>
		This option allows the user to generate specific coefficient files 
		if such files are reported as missing files by the tool. Note that 
		the generated coefficient files are used for the same user-defined 
		replacement policy specified in usr_rp.in. The user can specify a range 
		of average n for a particular d by setting the nxmin and nxmax values.
		If a single coefficient file with a specific nx value is expected to
		be generated, user can simply set the same value for nxmin and nxmax.
		
		Note: This option allows the user to use any scripts to generate
		coefficient files for different d values with a range of nxmin and 
		nxmax . It is recommended that these scripts are run on 
		multi-processors systems in order to greatly reduce the total run-time
		of generating those coefficient files.

	EXAMPLE 1:
		./acapp -n
		
	EXPECTED OUTPUT
		Creating new replacement policy...

		Coefficient file(s) added to: ./fine/<rpname>/d_0

 		Coefficient file(s) added to: ./fine/<rpname>/d_1

		Coefficient file(s) added to: ./fine/<rpname>/d_2

		Coefficient file(s) added to: ./fine/<rpname>/d_3

		Coefficient file(s) added to: ./fine/<rpname>/d_4

		Coefficient file(s) added to: ./fine/<rpname>/d_5

		Coefficient file(s) added to: ./fine/<rpname>/d_6

		Coefficient file(s) added to: ./fine/<rpname>/d_7

		Coefficient file(s) added to: ./fine/<rpname>/d_8

		Coefficient file(s) added to: ./fine/<rpname>/d_9

		Coefficient file(s) added to: ./fine/<rpname>/d_10

		Coefficient file(s) added to: ./fine/<rpname>/d_11
		Replacement policy: '8 - <rpname>' Added Successfully!	
		
		
		
	EXAMPLE 2:
		./acapp -n 4 5 7

	EXPECTED OUTPUT
		Adding coefficient file(s) to rp: <rpname>

		Coefficient file(s) added to directory: ./fine/<rpname>/d_4
		
		Note: the following three files will be created:
		      	./fine/<rpname>/d_4/<rpname>:4_4_5
		      	./fine/<rpname>/d_4/<rpname>:4_4_6
		      	./fine/<rpname>/d_4/<rpname>:4_4_7 
		      where in "<rpname>:4_4_5", the first digit "4" represents the 
		      coefficient file is for a cache with associativity of 4, 
		      the second digit "4" represents the d value, and the last digit
		      "5" represent the average n value.
		      
	EXAMPLE 3:
		./acapp -n 4 6 6

	EXPECTED OUTPUT
		Adding coefficient files to rp: <rpname>

		Coefficient file(s) added to directory: ./fine/<rpname>/d_4

		
		

OTHER CONFIGURATION FILES
-----------------------------

usr_rp.in
	This file serves as an input for a user-defined replacement policy.
	The file format is shown below:
	
	NAME newrp
	ASSOC 4
	PROB
	0.25 0.25 0.25 0.25
	
	WARNING: The format of this file should not be changed.
	The only values that can be changed are those that are not in uppercase.
	To avoid input errors, it is also necessary to create probability values 
	for each associativity.  For example, for an cache with associativity of 4, 
	there should be 4 probability values. Furthermore, the sum of probability 
	values should be equal to 1.
	

	
rplist.log
	This file serves as a logfile of all the supported replacement policies that
	allow for miss rate prediction. This file is maintained by the acapp tool 
	and the user is not supposed to modify it in most of cases.
	The file format is shown below:
	
	1 - NMRU4
	2 - NMRU1
	3 - LRUskw
	4 - MRUskw

	WARNING: The format of this file should not be changed.
	
	The index values are used to represent the replacement policy chosen for 
	miss rate prediction. For example 1 represents NMRU4, 2 for NMRU1, and so 
	forth. When user-defined replacement policies are created they are stored 
	in this file and given an index value.
	
	

RELATED ISSUES
-------------------------------
How to remove an existing replacement policy:
	In order to remove a replacement policy the user firstly needs to open the 
	rplist.log file and remove the entire line representing the replacement 
	policy. eg.  if NMRU4 is to be removed, the line:
		1 - NMRU4 
	should be completely deleted from rplist.log.
	Lastly, the user will need to access the ./fine/ from the acapp home 
	directory and delete the sub-directory that represents the replacement 
	policy, eg. if NMRU4 is to be removed, the directory "NMRU4"
	should be deleted from ./fine/
	

REFERENCES
------------------------------

All publications can be found in http://www.ece.ncsu.edu/arpers/index_files/Page620.htm

[1]  Fei Guo and Yan Solihin, An Analytical Model for Cache Replacement Policy 
     Performance, Proc.of ACM SIGMETRICS/Performance 2006 Joint International 
     Conference on Measurement and Modeling of Computer System (SIGMETRICS), 
     Saint-Malo, France, June 2006

[2]  Dhruba Chandra, Fei Guo, Seongbeom Kim, and Yan Solihin, Predicting Inter-Thread
     Cache Contention on a Chip Multi-Processor Architecture,  Proc. of the 11th 
     International Symposium on High Performance Computer Architecture (HPCA),  
     San Francisco, Feb 2005. 	


For more information, please feel free to contact Fei Guo (fguo@ncsu.edu) or 
Yan Solihin (solihin@eos.ncsu.edu)
	


	

