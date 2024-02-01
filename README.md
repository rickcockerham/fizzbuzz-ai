# README

This is my example code project.  It's a Ruby and C program that learns the Fizz Buzz problem.   Fizz Buzz is a program that prints the numbers from 1 to 100 and for multiples of '3' print 'Fizz' instead of the number and for the multiples of '5' print 'Buzz' and multiples of both '3' and '5' print 'FizzBuzz'

I realize this problem is extremely easy to solve and there is a lot of information on how to solve it efficiently, but I decided to do it the hard way.  This program lets a neural network learn the solution.

I wrote the neural network code in C and the web interface in Ruby on Rails.  My working AI can be viewed here https://www.ai-stocktrading.com

This program shows my skill with the following technologies:

Ruby on Rails (2.5.3, 6.0.7)  
C (posix)  
Machine Learning (a simple backpropagating, gradient descent, neural network)  
RubyC (A system to make C methods available in Ruby.)  

The example was deployed to a linode.com node.  However, I have also worked with Rackspace, Google, Linode, and AWS.

Apache  
Passenger  
iptables (do you feel lucky, punk?  Go ahead, check my ports.)  

This is obviously not a fully functioning git repo that you can simply download and run.  However everything is there if you know what you're doing.  The working solution is deployed to the link above.
The interesting files:  
app/fbai/fbai.c  
app/controllers/index_controller.rb  
app/views/index/index.html.erb  

No rspecs?  I didn't feel it was appropriate to include a test since the C program has to run a variable number of times and tests itself before sending the output to the view.  My 'test' is around fbai.c line 140.

Rick
