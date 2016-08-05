//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  CODE FILENAME: BatzelAssn5.cpp
//  DESCRIPTION:   This program is designed to keep track of rental car locations within a parking garage. If there is no data stored, no input file, the program
//				   will initialize a 2D array to a completely full state. If there is an input file then it will be loaded into in the array. After either of
//				   processes have occured, the user will be allowed to rent or return vehicles to and from the garage. If rented, the vehicle on the topmost
//				   floor will be removed first. If returned, the vechilce can be parked on any floor that is not already full. Upon exiting the program, a binary
//				   file will be written with whatever data is stored in the array.
//  CLASS/TERM:    CS362 / 14S8W1
//  DESIGNER:      Andrew C. Batzel
//  FUNCTIONS:     main- The main function attempts to open a binary file and loops through the menu making function calls to the menu choices.
//				   notGarageFile- This function is called only when the binary file that contains the data for the garage doesn't exist. It will initialize all
//								  an array that stores vehicle locations to being completely full.
//				   garageFile- This function is called after sucessfully opening a binary file. First it will initialize the array to empty, then read in data
//							   from the file into the array.
//				   garageStatus- This function is a simple loop that will output to the display how many vehicles of each type are on each level of the garage.
//				   actionMenu- This function prompts the user to choose whether to rent, turn in a vehicle or exit the program while verifing the correctness of
//							   of the users selection.
//				   subMenu- The subMenu is called when the user chooses to rent or turn in a vehicle and prompts the user to choose which type of vehicle to rent
//						    or return while verifiying the correctness of the users selection.
//				   turnInRental- This function runs through a loop to count all the cars of the chosen type to return back into the garage. Since only 5 cars of
//							     one type are allowed on each level at a time, if the level is full a bool value will be evaluated to not allow the user to
//							     turn in that vehicle to a full level.
//				   checkOutRental- This function runs through a loop to count all the cars of the chosen type to return back into the garage. If the count results
//                                 in a empty value, this function will not allow the user to check out a car that doesn't exist.
//				   exitProgram- Upon exiting the program this function will write all the data stored in the garage array into a binary file.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cctype>
#include <stdlib.h>

using namespace std;

// global constants
const int FULL = 5,             //  maxinum amout of vehicles that can be stored on a level for a each vehicle type
          EMPTY = 0,            // minumum amount of vehicles that can be stored on a level for each vehicle type
          LEVELS = 6,           // the number of levels including the basement
          CAR_TYPES = 4;        // the number of vechile types
enum carSize {COMPACT, MIDSIZE, FULLSIZE, SUV, NONE};
const string CAR_SIZE[] = {"Compact", "Midsize", "Fullsize", "SUV"};

// function prototypes
void notGarageFile(int garage[][CAR_TYPES]);
void garageFile(ifstream& dataIn,int garage[][CAR_TYPES]);
void garageStatus(int garage[][CAR_TYPES], int& allTotal);
char actionMenu(int allTotal);
carSize subMenu(string actionType);
void turnInRental(int garage[][CAR_TYPES], carSize carReturned);
void checkOutRental(int garage[][CAR_TYPES], carSize carRented);
void exitProgram(int garage[][CAR_TYPES]);

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     main
//  DESCRIPTION:  The main function attempts to open a binary file and loops through the menu making function calls to the menu choices.
//  OUTPUT:
//       Return Val:   Returns the value 0 upon sucessful completion of the program.
//  CALLS TO:     notGarageFile
//                garageFile
//                garageStatus
//                actionMenu
//                subMenu
//                turnInRental
//                checkOutRental
//                exitProgram
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

int main()
{
    int garage[LEVELS][CAR_TYPES],
        allTotal = 0;
    carSize subSelection;
    char menuSelection;
    string actionType;
   
    //  initial description of program
    cout << "This program is designed to keep track of rental car locations within a parking garage." << endl << endl;
    system("pause");
    system("cls");
    
    ifstream dataIn;
    //  open binary file that contains data about the vehicles that are in the garage
    dataIn.open("GARAGE.bin", ios::in | ios:: binary);
    //  file does not exist
    if (!dataIn)
       notGarageFile(garage);
    //  file does exist
    else
        garageFile(dataIn, garage);

    do
    {            
        system("cls");
        garageStatus(garage, allTotal);
        menuSelection = actionMenu(allTotal);
        if (menuSelection != 'E')
        {
	        if ( (menuSelection == 'R') || (menuSelection == 'T') )
	        {
	             if (menuSelection == 'R')
	                actionType = "rent";
	             else
	                 actionType = "return";
	                 
	             subSelection = subMenu(actionType);
	             
	             if ( (menuSelection == 'R') && (subSelection != NONE) )
	                checkOutRental(garage, subSelection);
	             
	             if ( (menuSelection == 'T') && (subSelection != NONE) )
	                  turnInRental(garage, subSelection);
	        } //  end if user want to return or rent a vehicle
	    } // end if user does not choose to exit
    }while (menuSelection != 'E');
    
    exitProgram(garage);
             

    system("pause");
    return 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     notGarageFile
//  DESCRIPTION:  This function is called only when the binary file that contains the data for the garage doesn't exist. It will initialize all
//				  an array that stores vehicle locations to being completely full.
//  INPUT:
//     Parameters:   garage- a 2D parameter used to store the number of each type of vehicle on all levels of the garage.
//  OUTPUT:
//     Parameters:   garage- arrays are passed via reference by default so it will be used as input and output.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void notGarageFile(int garage[][CAR_TYPES])
{
     //  initialize the garage array to full
     for (int carType = 0; carType < NONE; carType++)
     {
         //  set all levels to full
         for (int garageLevel = 0; garageLevel < LEVELS; garageLevel++)
             garage[garageLevel][carType] = FULL;
     } // increment through each vehicle Type
     
     return;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     garageFile
//  DESCRIPTION:  This function is called after sucessfully opening a binary file. First it will initialize the array to empty, then read in data
//				  from the file into the array.
//  INPUT:
//     Parameters:   garage- a 2D parameter used to store the number of each type of vehicle on all levels of the garage.
//     File:         dataIn- reads from a binary file that contains data about the number of vehicles in the garage.
//  OUTPUT:
//     Parameters:   garage- arrays are passed via reference by default so it will be used as input and output    . 
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void garageFile(ifstream& dataIn, int garage[][CAR_TYPES])
{
     int lvlIndex,        // stores the level from the binary file
         rowIndex,        // stores the vechile type or row from the binary file
         carsParked;      // stores the amount of vehicles from each type and level from the binary file
     
     //  initialize the garage array to empty
     for (int carType = 0; carType < NONE; carType++)
     {
         //  set all levels to empty
         for (int garageLevel = 0; garageLevel < LEVELS; garageLevel++)
             garage[garageLevel][carType] = EMPTY;
     } // increment through each vehicle Type
     
     dataIn.read (reinterpret_cast<char*>(&lvlIndex), sizeof(int) );
     dataIn.read (reinterpret_cast<char*>(&rowIndex), sizeof(int) );
     dataIn.read (reinterpret_cast<char*>(&carsParked), sizeof(int) );
     while(dataIn)
     {
         garage[lvlIndex][rowIndex] = carsParked;
         dataIn.read (reinterpret_cast<char*>(&lvlIndex), sizeof(int) );
         dataIn.read (reinterpret_cast<char*>(&rowIndex), sizeof(int) );
         dataIn.read (reinterpret_cast<char*>(&carsParked), sizeof(int) );
     } //  end while there is data to be read from the binary file
     
     dataIn.close();
     
     cout << "All file data has been read." << endl << endl;
     
     system("pause");     
     return;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     garageStatus
//  DESCRIPTION:  This function is a simple loop that will output to the display how many vehicles of each type are on each level of the garage.
//  INPUT:
//     Parameters:   garage- a 2D parameter used to store the number of each type of vehicle on all levels of the garage.
//                   allTotal- keeps count of the total number of vehicles in the garage.
//  OUTPUT:
//     Parameters:   garage- arrays are passed via reference by default so it will be used as input and output.
//                   allTotal- passed via reference, keeps count of the total number of vehicles in the garage.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void garageStatus(int garage[][CAR_TYPES], int& allTotal)
{
     int typeTotal;        //  counts the total number of vehicles for each type of vehicle
     
     allTotal = 0;         //  initialize the total number of vehicles to 0
     
     cout << left << setw(15) << "Level" << right << setw(5) << 0 << setw(5) << 1 << setw(5) << 2 << setw(5) << 3 << setw(5) << 4 << setw(5) << 5 << setw(10) << "Total" << endl << endl;
     
     for (int carType = COMPACT; carType < NONE; carType++)
     {
         cout << left << setw(15) << CAR_SIZE[carType];
         typeTotal = 0;
         
         for (int garageLevel = 0; garageLevel < LEVELS; garageLevel++)
         {
             cout << right << setw(5) << garage[garageLevel][carType];
             typeTotal += garage[garageLevel][carType];
         } // end for looping through the garage levels
         
         cout << right << setw(10) << typeTotal << endl;
         allTotal += typeTotal;
     } //  end for looping through the vehicle types
     
     cout << left << setw(50) << "" << setw(5) << "-----" << endl
          << left << setw(45) << "Total Cars Available for Rental" << right << setw(10) << allTotal << endl;
     
     return;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     actionMenu
//  DESCRIPTION:  This function prompts the user to choose whether to rent, turn in a vehicle or exit the program while verifing the correctness of
//				  of the users selection.
//  INPUT:
//     Parameters:   allTotal- the total number of vehicles in the garage.
//  OUTPUT:       
//     Return Val:   userAction- the character value the user has selected from the menu. 
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

char actionMenu(int allTotal)
{
     char userAction;                        //  stores the character value of the users selection from the menu options
     bool validInput = true,                 //  initialize the validInput bool to true
          canRent = true,                    // initialize the canRent bool to true
          canReturn = true;                  // initialize the canReturn bool to true
     //  if the number of vehicles in the array is equal to maximum allowable vechiles you are not allowed to return a vehicle, set canReturn to false
     if (allTotal == CAR_TYPES * LEVELS * FULL)
        canReturn = false;
     //  if there are no vehicles in the array you can't rent a vehicle, set canRent to false
     if (allTotal == 0)
        canRent = false;
     
     cout << left
          << "------------------------------Action Menu------------------------------" << endl << endl;
     if (canRent)
        cout << "R - Rent a car" << endl;
     if (canReturn)
        cout << "T - Turn in a car" << endl;
     cout << "E - Exit the program" << endl << endl;
          
     do
     {
         if (!validInput)
            cout << "ERROR - Invalid character selection. Try again." << endl << endl; 
         cout << "Choose an option from above: ";
         cin >> userAction;
         userAction = toupper(userAction);
         // validate input    
         if (!canRent && (userAction == 'R'))
            validInput = false;
        else if (!canReturn && (userAction == 'T'))
        	 validInput = false; 
		 else if ( (userAction != 'R') && (userAction != 'T') && (userAction != 'E') )
             validInput = false;
         else
             validInput = true;
     }while (!validInput);
     
     return userAction;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     subMenu
//  DESCRIPTION:  The subMenu is called when the user chooses to rent or turn in a vehicle and prompts the user to choose which type of vehicle to rent
//				  or return while verifiying the correctness of the users selection.
//  INPUT:
//     Parameters:   actionType- the selected menu option from the actionMenu function.
//  OUTPUT:
//     Return Val:   valueReturned- the enum carSize value selected from the user.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

carSize subMenu(string actionType)
{
    char userAction;              //  stores the character value of the users selection from the subMenu options
    carSize valueReturned;        //  stores the enum carSize value to be returned from the subMenu
    bool validInput = true;       //  initialize the validInput bool to true
    
    cout << endl << endl
          << "C - Compact" << endl
          << "M - Mid-Size" << endl
          << "F - Full Size" << endl
          << "S - SUV" << endl
          << "R - Return to main menu" << endl << endl;
     do
     {
        if (!validInput)
           cout << "ERROR - Invalid character selection. Try again." << endl << endl;
        cout << "Choose type of car to " << actionType << ", or return to the main menu: ";
        cin >> userAction;
        userAction = toupper(userAction);
         // validate input    
         if (!isalpha(userAction))
            validInput = false;
         else if ( (userAction != 'C') && (userAction != 'M') && (userAction != 'F')  && (userAction != 'S') && (userAction != 'R'))
             validInput = false;
         else
             validInput = true;
     }while (!validInput);
     
     switch (userAction)
     {
            case 'C':
                 valueReturned = COMPACT;
                 break;
            case 'M':
                 valueReturned = MIDSIZE;
                 break;
            case 'F':
                 valueReturned = FULLSIZE;
                 break;
            case 'S':
                 valueReturned = SUV;
                 break;
            default:
                    valueReturned = NONE;
     } // end switch to set user selection subMenu options to a enum carSize value
    
    return valueReturned;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     turnInRental
//  DESCRIPTION:  This function runs through a loop to count all the cars of the chosen type to return back into the garage. Since only 5 cars of
//				  one type are allowed on each level at a time, if the level is full a bool value will be evaluated to not allow the user to
//				  turn in that vehicle to a full level.
//  INPUT:
//     Parameters:   garage- a 2D parameter used to store the number of each type of vehicle on all levels of the garage.
//                   carReturned- the enum carSize value returned from the subMenu Function.
//  OUTPUT:
//     Parameters:   garage- arrays are passed via reference by default so it will be used as input and output.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void turnInRental(int garage[][CAR_TYPES], carSize carReturned)
{
     int typeTotal = 0,                    //  initialize the total number of vehicles for a particular type to 0
     	 returnLevel = 0;                  //  initialize the returnLevel variable to 0
     bool spaceAvailable[LEVELS],          //  array of bools used to store values based on if there is any parking space available on a particular garage level
          wrongCompany = false,            //  initialize wrongCompany bool to false
          rowFullBool,                     //  bool used to state whether a row is full or not
          validInput;                      //  bool used to state whether an input is valid or not
     
     cout << "Number of " << CAR_SIZE[carReturned] << " Cars Parked on each Level" << endl << endl
          << setw(15) << "Level:" << right << setw(5) << 0 << setw(5) << 1 << setw(5) << 2 << setw(5) << 3 << setw(5) << 4 << setw(5) << 5 << setw(10) << "Total" << endl << endl
          << left << setw(15) << CAR_SIZE[carReturned];
     for (int garageLevel = 0; garageLevel < LEVELS; garageLevel++)
     {
         cout << right << setw(5) << garage[garageLevel][carReturned];
         if (garage[garageLevel][carReturned] == FULL)
            spaceAvailable[garageLevel] = false;
         else
             spaceAvailable[garageLevel] = true;
         typeTotal += garage[garageLevel][carReturned];
     } //  end for looping through garage levels
     
     cout << setw(10) << typeTotal << endl << endl;
     
     if ( !spaceAvailable[0] && !spaceAvailable[1] && !spaceAvailable[2] && !spaceAvailable[3] && !spaceAvailable[4] && !spaceAvailable[5] )
        wrongCompany = true;
        
     if (wrongCompany)
        cout << "Car attempting to be returned belongs to another company." << endl;
     else
     {
         rowFullBool = true;
         validInput = false;
         while (rowFullBool || !validInput)
         {                
             cout << "Car may be turned in to any level that has fewer than 5 cars." << endl << endl
                  << "Which level will the car be parked on? ";
             cin >> returnLevel;
             cout << endl;
             // validate input
             if (cin)
             {             
	             if ( (returnLevel >= 0) && (returnLevel <= (LEVELS - 1) ) )
	                  validInput = true;
	             else
	                 validInput = false;

	             if (validInput)
	             {                 
	                 if (spaceAvailable[returnLevel])
	                    rowFullBool = false;
	                 else
	                     rowFullBool = true;
	                
	                 if (rowFullBool)
	                    cout << "Error - Level " << returnLevel << " already contains 5 cars." << endl << endl;
	             } // end if input is valid
	         } //  end if user enters integer value
             else
             {
                cout << "ERROR - Invalid level selection. Try again." << endl << endl;
                cin.clear();
				cin.ignore(256, '\n');
             } // end else if user does not enter an integer value     
         } //  end while the row is rull or not a valid input selected
         
         typeTotal++;
         garage[returnLevel][carReturned]++;
         
         cout << "Okay to park car in " << CAR_SIZE[carReturned] << " row of level " << returnLevel << endl
              << typeTotal << " " << CAR_SIZE[carReturned] << " cars will now be available." << endl;
     }
         
         system("pause");
         return;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     checkOutRental
//  DESCRIPTION:  This function runs through a loop to count all the cars of the chosen type to return back into the garage. If the count results
//                in a empty value, this function will not allow the user to check out a car that doesn't exist.
//  INPUT:
//     Parameters:   garage- a 2D parameter used to store the number of each type of vehicle on all levels of the garage.
//                   carRented- the enum carSize value returned from the subMenu Function.
//  OUTPUT:
//     Parameters:   garage- arrays are passed via reference by default so it will be used as input and output.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void checkOutRental(int garage[][CAR_TYPES], carSize carRented)
{
     bool rentAvailable[LEVELS],             //  array of bools used for if there are vehicles available for rent on a garage level
          levelHit = false;                  //  initialize levelHit bool to false, levelHit is used for finding if a garage level can have vehicles removed from it
     int typeTotal = 0,                      //  initialize typeTotal to 0
         pickUpLevel = LEVELS - 1;           //  initialize pickUpLevel to one less than the amount of levels, used for indexing the garage array
     
     cout << endl;
     cout << setw(15) << "Level:" << right << setw(5) << 0 << setw(5) << 1 << setw(5) << 2 << setw(5) << 3 << setw(5) << 4 << setw(5) << 5 << setw(10) << "Total" << endl << endl
          << left << setw(15) << CAR_SIZE[carRented];
     for (int garageLevel = 0; garageLevel < LEVELS; garageLevel++)
     {
         cout << right << setw(5) << garage[garageLevel][carRented];
         if (garage[garageLevel][carRented] == EMPTY)
            rentAvailable[garageLevel] = false;
         else
             rentAvailable[garageLevel] = true;
         typeTotal += garage[garageLevel][carRented];
     } //  end for looping through the garage levels
     cout << setw(10) << typeTotal << endl << endl;
     if (!rentAvailable[0] && !rentAvailable[1] && !rentAvailable[2] && !rentAvailable[3] && !rentAvailable[4] && !rentAvailable[5])
        cout << "There are no " << CAR_SIZE[carRented] << " cars available for rent at this moment." << endl;
     else
     {
         do
         {
             if (rentAvailable[pickUpLevel])
                levelHit = true;
             else
                 pickUpLevel--;
         }while (!levelHit);
         
         garage[pickUpLevel][carRented]--;
         typeTotal--;
         
         cout << "Pick up car from " << CAR_SIZE[carRented] << " row of level " << pickUpLevel << endl
              << typeTotal << " " << CAR_SIZE[carRented] << " cars will now be available." << endl;
     }
     
     system("pause");
     return;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     exitProgram
//  DESCRIPTION:  Upon exiting the program this function will write all the data stored in the garage array into a binary file.
//  INPUT:
//     Parameters:   garage- a 2D parameter used to store the number of each type of vehicle on all levels of the garage.
//  OUTPUT:
//     Parameters:   garage- arrays are passed via reference by default so it will be used as input and output.
//     File:         dataOut- writes data from the garage array to a binary file.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void exitProgram(int garage[][CAR_TYPES])
{
     ofstream dataOut;
    
     dataOut.open("GARAGE.bin", ios::out | ios:: binary);
    
     for (int carType = COMPACT; carType < NONE; carType++)
     {         
         for (int garageLevel = 0; garageLevel < LEVELS; garageLevel++)
         {
             if (garage[garageLevel][carType] != EMPTY)
             {
                 dataOut.write (reinterpret_cast<char*>(&garageLevel), sizeof(int) );
                 dataOut.write (reinterpret_cast<char*>(&carType), sizeof(int) );
                 dataOut.write (reinterpret_cast<char*>(&garage[garageLevel][carType]), sizeof(int) );
             } //  end if the garage level is not empty
         } //  end for looping through garage levels
     } //  end for looping through car types
     
     dataOut.close();
     
     return;
}

         
