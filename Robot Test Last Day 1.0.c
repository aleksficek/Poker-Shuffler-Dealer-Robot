const int SIZE = 8;     // Global constant represents max players
const int WIDTH_VALUE = 30; 	// Global constant represents average width of player (degrees)

// Ultrasonic Function:
int utlra (int * position)
{
	nMotorEncoder[motorA] = 0;
	int count = 0;		// count returns number of players
	resetGyro(S2);

	while  (nMotorEncoder[motorA] <= 360 && count < SIZE)
	{
		motor[motorA] = 5;

		if (SensorValue[S1] < 60)
		{
			position[count] = getGyroDegrees(S2);
			motor[motorA] = 0;
			wait1Msec(2000);

			while ( getGyroDegrees(S2) < (position[count] + WIDTH_VALUE) ||  SensorValue[S1] < 60)
				motor[motorA] = 5;

			count++;
		}
	}
	motor[motorA] = 0;
	return count;
}

// Randomizer Function:
void random_deal ()
{
	int x = 0;
	x = random(1);
	if (x == 1)		// Powers left side motors
	{
		motor[motorC] = -13;
		wait1Msec(1420);
		motor[motorC] = 15;
		wait1Msec(1500);
		motor[motorC] = 0;
	}
	else					// Powers right side motors
	{
		motor[motorD] = 13;
		wait1Msec(1420);
		motor[motorD] = -15;
		wait1Msec(1500);
		motor[motorD] = 0;
	}
}

// Card Shooter Function:
void card_shoot ()
{
	motor[motorB] = -60;
	wait1Msec(900);
	motor[motorB] = 0;
}

// Rotating Function:
void rotate(int * position, int number_players)
{
	resetGyro(S2);

	for (int deal = 0; deal < number_players; deal++)
	{
		while (getGyroDegrees(S2) < (position[deal] + (WIDTH_VALUE/2))) // Rotates to the midpoint of the player
			motor[motorA] = 8;

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

// Flop Function:
void flop ()
{
	burn();
	for (int flop = 0; flop < 3; flop++)
	{
		random_deal ();
		wait1Msec(300);
		card_shoot ();
	}
}

// Turn and River Function:
void turn_river ()
{
	for (int turn_river = 0; turn_river < 2; turn_river++)
	{
		while (SensorValue[S3] == 0) {}
		burn();
		random_deal();
		wait1Msec(300);
		card_shoot ();
	}
}

// Output Function:
void output (int number_players, int time_1, int time_2, int number_hands)
{
	const int MILSEC_CON = 1000, TIME_CON = 60;

	int time1Sec = time_1 /MILSEC_CON;
	int time1Min = time1Sec/TIME_CON;
	int time1SecFormatted = time1Sec - time1Sec/TIME_CON * TIME_CON;

	int time2Sec = time_2 /MILSEC_CON;
	int time2Min = time2Sec /TIME_CON;
	int time2SecFormatted = time2Sec - time2Sec/TIME_CON * TIME_CON;

	displayString(0, "Number of players: %d", number_players);
	displayString(1, "Game Time: %d min and %d sec", time1Min, time1SecFormatted);
	displayString(2, "Hand Time: %d min and %d sec", time2Min, time2SecFormatted);
	displayString(3, "Number of Hands: %d", number_hands);
	displayString(5, "Press ENTER to end");
	displayString(6, "Press RIGHT if players left");
	displayString(7, "Press other to play next hand");
}

task main()
{
	SensorType[S1] = sensorEV3_Ultrasonic;
	wait1Msec(50);
	SensorType[S2] = sensorEV3_Gyro;
	wait1Msec(50);
	SensorMode[S2] = modeEV3Gyro_RateAndAngle;
	wait1Msec(50);
	SensorType[S3] = sensorEV3_Touch;
	wait1Msec(50);
	resetGyro(S2);

	// Wait for button press to start program
	while (!getButtonPress(buttonAny)) {}
	while (getButtonPress(buttonAny)) {}

	// Declare array of players and variables for output function
	int position[SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};
	int number_hands = 0; int time_1 = 0; int time_2 = 0;

	// Call Ultrasonic function and return number of players
	int number_players = utlra(position);

	// Clear timer to record time since start of game
	clearTimer(T1);

	while (!getButtonPress(buttonEnter))
	{
		// Recount number of players/positons with ultrasonic function if someone leaves game
		if (getButtonPress(buttonRight))
		{
			for (int reset = 0; reset < SIZE; reset++)
				position[reset] = 0;
			number_players = utlra(position);
		}

		resetGyro(S2);
		// Clear timer to record time since start of each hand
		clearTimer(T2);

		for (int card_mode = 0; card_mode < 2; card_mode++)		// Loop ensures that each player gets dealt two cards
			rotate(position, number_players);

		while (SensorValue[S3] == 0) {}

		// Call flop function (deal three cards)
		flop();

		// Call turn and river function (burns and deals twice)
		turn_river();

		number_hands++;
		time_1 = time1[T1];
		time_2 = time1[T2];

		// Call output function
		output(number_players,time_1, time_2,  number_hands);

		// Press button to shutdown, continue, or recount/continue
		while (!getButtonPress(buttonAny)) {}
	}
}
