require 'fbai'

class IndexController < ApplicationController
  layout false
  
  # I need the AI to survive between calls to the testit function.
  $ai = Fbai.new

  def index
    #reset the AI
    $ai.reinit();
  end

  # this will return a string of numbers and their value (x,fizz,buzz,fizzbuzz)
  def testit
    ret = '';
    for x in 1..100
      data = $ai.valueat(x).to_i
      ret << "#{x}="
      if(-1 == data)
	ret << '-err-'
      elsif(0 == data)
      	ret << "#{x}"
      elsif(2 != data)
      	ret << 'FIZZ'
      end
      if(1 < data)
      	ret << 'BUZZ'
      end
      ret << " "
    end
    render html: ret, layout: false
  end

  # this tells the AI to do one training pass.  Then the testit function is called again to read the results.
  def trainmore
    $ai.gotrain()
    render plain: 'success'
  end

end
