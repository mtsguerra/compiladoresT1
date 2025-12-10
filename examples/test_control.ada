-- Test control structures
-- Note: This is a simplified Ada subset that uses implicit variable declarations
-- Variables are automatically declared on first use
procedure TestControl is
begin
    x := 15;
    
    if x > 10 then
        Put_Line("x is greater than 10");
    else
        Put_Line("x is not greater than 10");
    end if;
    
    counter := 0;
    while counter < 5 loop
        Put_Line(counter);
        counter := counter + 1;
    end loop;
    
    Put_Line("Done");
end TestControl;
