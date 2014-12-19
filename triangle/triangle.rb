
triangle = File.readlines( "triangle.txt" )

def get_greatest_inls
  _bottom_two_rows( second_to_last_row, last_row )
  row_length = second_to_last_row.length
  new_last_row = Array.new( row_length, nil )

  row_length.times do |i|
    new_num1 = second_to_last_row[i].to_i + last_row[i].to_i
    new_num2 = second_to_last_row[i].to_i + last_row[i + 1].to_i
    new_last_row[i] = [new_num1, new_num2].max
  end
  new_last_row
end


def get_greatest_route_qty( triangle )
  triangle = triangle.map { |row| row.split( " " )}
  length = triangle.length
  i = length - 1
  until i == 0
    second_to_last_row = triangle[i - 1]
    last_row = triangle[i]
    triangle[i - 1] = get_greatest_inls
    _bottom_two_rows( second_to_last_row, last_row )
    i -= 1
  end
    p triangle.first.first
end


get_greatest_route_qty( triangle )
