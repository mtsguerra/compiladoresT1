-- Comprehensive test demonstrating all compiler features
-- Note: This is a simplified Ada subset that uses implicit variable declarations
procedure Comprehensive is
begin
    -- Basic arithmetic
    a := 10;
    b := 20;
    sum := a + b;
    diff := b - a;
    product := a * b;
    quotient := b / a;
    
    Put_Line("=== Arithmetic Tests ===");
    Put_Line("10 + 20 =");
    Put_Line(sum);
    Put_Line("20 - 10 =");
    Put_Line(diff);
    Put_Line("10 * 20 =");
    Put_Line(product);
    Put_Line("20 / 10 =");
    Put_Line(quotient);
    
    -- Relational operators
    Put_Line("=== Relational Tests ===");
    x := 15;
    
    if x > 10 then
        Put_Line("15 > 10: true");
    end if;
    
    if x < 20 then
        Put_Line("15 < 20: true");
    end if;
    
    -- Logical operators
    Put_Line("=== Logical Tests ===");
    result1 := x > 10 and x < 20;
    if result1 = 1 then
        Put_Line("10 < 15 < 20: true");
    end if;
    
    -- Nested if-else
    Put_Line("=== Nested Control Structures ===");
    score := 85;
    
    if score >= 90 then
        Put_Line("Grade: A");
    else
        if score >= 80 then
            Put_Line("Grade: B");
        else
            if score >= 70 then
                Put_Line("Grade: C");
            else
                Put_Line("Grade: D");
            end if;
        end if;
    end if;
    
    -- While loop
    Put_Line("=== Loop Test ===");
    Put_Line("Countdown from 5:");
    counter := 5;
    while counter > 0 loop
        Put_Line(counter);
        counter := counter - 1;
    end loop;
    
    Put_Line("=== All Tests Complete ===");
end Comprehensive;
