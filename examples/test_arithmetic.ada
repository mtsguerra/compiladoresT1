-- Test arithmetic operations
-- Note: This is a simplified Ada subset that uses implicit variable declarations
-- Variables are automatically declared on first use
procedure TestArithmetic is
begin
    a := 10;
    b := 20;
    sum := a + b;
    diff := a - b;
    prod := a * b;
    quot := b / a;
    
    Put_Line("Sum:");
    Put_Line(sum);
    Put_Line("Difference:");
    Put_Line(diff);
    Put_Line("Product:");
    Put_Line(prod);
    Put_Line("Quotient:");
    Put_Line(quot);
end TestArithmetic;
