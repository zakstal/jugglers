file = File.readlines( "jugglers_three.rb" )

file.each_with_index do |line, line_ind|
  line.each_char.with_index do |ch, ch_ind|
    if line =~ /\((?=\S)/
      line.insert( " ", ch_ind + 1 )
      puts line
      gets
    end
  end
end
