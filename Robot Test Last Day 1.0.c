const int SIZE = 8;     // global constant

// Ultrasonic Function:
int utlra (int * position)	// count finds the number of players
{
	nMotorEncoder[motorA] = 0;
	int count = 0;

	resetGyro(S2);

	while  (nMotorEncoder[motorA] <= 360 && count < SIZE) 	// or until max players (8) counted
	{
		 motor[motorA] = 5;

		if (SensorValue[S1] < 60)
		{
			// motor[motorA] = 0;
			// wait1Msec(500);
			position[count] = getGyroDegrees(S2);

			displayString(count, "Position: %f", position[count]);

			motor[motorA] = 0;
			wait1Msec(2000);

			while ( getGyroDegrees(S2) < (position[count] + 30) ||  SensorValue[S1] < 60)
			{
				motor[motorA] = 5;
		  }
			// motor[motorA] = 0;
			// wait1Msec(1000);
			count++;
		}
	}
	motor[motorA] = 0;
	return count;
}

// Randomizer Function:
void random_deal ()
{
	// Assume motor C represents 1st card pile motor and motor D represents 2nd card pile motor
	int x = 0;
	x = random(1);
	// Display x
	if (x == 1)
	{
		motor[motorC] = -13;
		wait1Msec(1420);    // test to find correct motor and times values
		motor[motorC] = 15;
		wait1Msec(1500);
		motor[motorC] = 0;
	}
	else
	{
		motor[motorD] = 13;
		wait1Msec(1420);    // test to find correct motor and times values
		motor[motorD] = -15;
		wait1Msec(1500);
		motor[motorD] = 0;
	}
}


// Card Shooter Function:
void card_shoot ()
{
	motor[motorB] = -60;	// power values and time values need to be tested for optimal performance
	wait1Msec(900);
	//motor[motorB] = 60;
	//wait1Msec(450);
	motor[motorB] = 0;
}

// Rotating Function:
void rotate(int * position, int number_players)
{
	resetGyro(S2);

	for (int deal = 0; deal < number_players; deal++)
	{
		while (getGyroDegrees(S2) < (position[deal] + 15))				// get rid of magic numbers
			motor[motorA] = 8;				// might need to add precision

		motor[motorA] = 0;
		random_deal();
		card_shoot();
		wait1Msec(600);
	}
	while (SensorValue[S2] < 360)
		motor[motorA] = 5;
	motor[motorA] = 0;
}



// Burn Function:
void burn()
{
	resetGyro(S2);
	random_deal();
	while (getGyroDegrees(S2) < 45)
		motor[motorA] = 4;

	motor[motorA] = 0;
	card_shoot ();

	while (getGyroDegrees(S2) > 0)
		motor[motorA] = -4;

	motor[motorA] = 0;

}

// Output Function:
void output (int number_players, int time_1, int time_2, int number_hands)
{
	displayString(0, "Number of players: %d", number_players);
	displayString(1, "Duration of Hand: %d", time_1);
	displayString(2, "Duration of Game: %d", time_2);
	displayString(3, "Number of Hands: %d", number_hands);
	displayString(5, "Press left to end game, press right if a player has left, or press enter for next hand");
	// format time appropriately
}

task main()
{
	// declare sensors (gyro, ultra, touch) check if proper slots for S1, S2, S3
	SensorType[S1] = sensorEV3_Ultrasonic;
	wait1Msec(50);
	SensorType[S2] = sensorEV3_Gyro;
	wait1Msec(50);
	SensorMode[S2] = modeEV3Gyro_RateAndAngle;
	wait1Msec(50);
	SensorType[S3] = sensorEV3_Touch;
	wait1Msec(50);

	resetGyro(S2);			// check if this sets initial position to 0

	// wait for button press to start program
  while (!getButtonPress(buttonAny)) {}
  while (getButtonPress(buttonAny)) {}
  clearTimer(T1);

  // declare array[8 in size] and variables
  int position[SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};
  int number_hands = 0;

  // call ultrasonic function
  int number_players = utlra(position);

  while (!getButtonPress(buttonEnter))
	{
		// recount players with ultrasonic function if someone leaves game
	  if (getButtonPress(buttonRight))  // must be fixed
	  {
	  	for (int reset = 0; reset < SIZE; reset++)
	  		position[reset] = 0;
		   number_players = utlra(position);
		 }
		resetGyro(S2);
		clearTimer(T2);
		for (int card_mode = 0; card_mode < 2; card_mode++)	  // can make 2 be a variable so that omaha can also be dealt
			 rotate(position, number_players);									// loops ensure that each player gets dealt two cards each

		// wait for touch
		while (SensorValue[S3] == 0) {}

	  // call burn function
		burn();

		// deal three cards
		for (int flop = 0; flop < 3; flop++)
		{
			random_deal ();
			wait1Msec(300);
		  card_shoot ();
		}

		// wait for touch, burn, shoot * 2
	  for (int turn_river = 0; turn_river < 2; turn_river++)
	  {
		  while (SensorValue[S3] == 0) {}
		  burn();
		  random_deal();
		  wait1Msec(300);
		  card_shoot ();
	  }

	  number_hands++;

		// call output function
	  // output(int number_players, int time1[T1], int time1[T2], int number_hands);

	  // press button to shutdown, continue, or recount&continue
		while (!getButtonPress(buttonAny)) {}
		getButtonPress(buttonAny);					// might need to add constant shutdown feature
	}
}
