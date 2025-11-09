-- Teste de operadores relacionais
procedure OperadoresRelacionais is
begin
  a := 10;
  b := 20;

  if a = b then
    Put_Line("Igual");
  end if;

  if a /= b then
    Put_Line("Diferente");
  end if;

  if a < b then
    Put_Line("Menor");
  end if;

  if a <= b then
    Put_Line("Menor ou igual");
  end if;

  if b > a then
    Put_Line("Maior");
  end if;

  if b >= a then
    Put_Line("Maior ou igual");
  end if;
end OperadoresRelacionais;