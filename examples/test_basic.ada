-- Test basic operations
-- Note: This is a simplified Ada subset that uses implicit variable declarations
-- Variables are automatically declared on first use
procedure TestBasic is
begin
    x := 10;
    y := x + 5;
    Put_Line("Value of y:");
    Put_Line(y);
end TestBasic;
