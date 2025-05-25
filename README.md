# Overview

This is a client server program to allow for the control of a plant care system on an esp32 that then controlls a pump and light. it also has inputs that tell 
the tank levels. it is made so that a plant could be taken care of vertualy while you are gone.

[Software Demo Video](https://youtu.be/kAibE8Z8RMg)

# Network Communication

i used the esp32 library to make a server to client system to network the esp32 and a browser.

i am using tcp for a wifi server with a port 80

i am using HTTP comunication

# Development Environment

I used the arduino ide and its librarys to develop this project.

The programing language i used was C++ with the time and wifi arduino librarys.

# Useful Websites

{Make a list of websites that you found helpful in this project}
* [Random nerd tutorials](https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/)

# Future Work

{Make a list of things that you need to fix, improve, and add in the future.}
* Conection to a phone app
* check the reliability and test all parts mostly the time as it doesnt seem quite right