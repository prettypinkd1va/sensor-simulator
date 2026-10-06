## 1. Why are deterministic readings preferable to random readings for this lab?

Deterministic readings make the program easier to test because the same input gives the same results every time. This makes it easier to check if the program is working correctly. Random readings could give different results each time, making testing harder.

## 2. Why must the failure sentinel be checked before numeric thresholds?

The failure sentinel means that the sensor has failed, not that the temperature is extremely high or low. It needs to be checked first so the program does not treat it like a normal temperature reading. This allows the program to correctly identify a sensor failure.

## 3. Why should a simulated sensor failure not automatically produce `EXIT_FAILURE`?

A simulated sensor failure is something the program is supposed to detect and report. It does not mean that the program itself failed. `EXIT_FAILURE` should be used when the program cannot run correctly, such as when invalid input is entered.

## 4. What role does the enum type play in readability and correctness?

The enum gives names to the different sensor modes and statuses. This makes the code easier to understand than using numbers by themselves. It also helps make sure the program uses the correct mode or status.

## 5. Which invalid input was easiest to overlook, and how does the program reject it?

The sample count of `0` was easy to overlook because it is still a number. However, the program requires the sample count to be between `1` and `100`. It rejects `0` and any number outside that range before running the simulation.
