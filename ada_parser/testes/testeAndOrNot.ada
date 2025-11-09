-- Teste de operadores AND OR e NOT
procedure OperadoresLogicos is
begin
  x := 15;
  y := 25;

  if x > 10 and y < 30 then
    Put_Line("Ambas condicoes verdadeiras");
  end if;

  if x < 5 or y > 20 then
    Put_Line("Pelo menos uma verdadeira");
  end if;

  if not x = 0 then
    Put_Line("X nao e zero");
  end if;

  if x > 5 and y > 20 and x < 20 then
    Put_Line("Tres condicoes");
  end if;
end OperadoresLogicos;