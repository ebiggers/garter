define i32 @is_odd(i32 %n) #0 {
  %1 = and i32 %n, 1
  ret i32 %1
}

define i32 @is_even(i32 %n) #0 {
  %1 = and i32 %n, 1
  %2 = xor i32 %1, 1
  ret i32 %2
}

define i32 @mul8(i32 %n) #0 {
  %1 = shl i32 %n, 3
  ret i32 %1
}
