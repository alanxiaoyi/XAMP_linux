#include "interface.h"

using namespace std;
int main(){

	parse_config(");


	cout<<"1"<<endl;
	
   pid_t pID = vfork();
   if (pID == 0){	
		char *args[] = {"./acapp", "-c", "-f1", "./csq/benchmark1.csq", "-f2", "./csq/benchmark2.csq", (char *) 0 }; 
		int i=execv("./acapp", args);
		if(-1==i){
			cout<<"fail to execute the model, please check the parameter"<<endl;
			_exit(1);
		}
	}
    else if (pID < 0){
        cout << "Failed to fork" << endl;
        exit(1);
        // Throw exception
    }
    else{
		int status;
		pid_t pid;
		pid=wait(&status);
		cout<<"2";
		cout<<pid<<endl;
    }
return 1;
}