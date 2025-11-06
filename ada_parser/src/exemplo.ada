procedure Main is
begin
  -- Exemplo de atribuições
  x := 10;
  y := x + 5;
  z := x * y - 3;

  -- Exemplo de condicional
  if y > 10 then
    Put_Line("Y é maior que 10");
  else
    Put_Line("Y não é maior que 10");
  end if;

  -- Exemplo de loop
  contador := 5;
  while contador > 0 loop
    Put_Line("Contando...");
    contador := contador - 1;
  end loop;

  -- Exemplo de I/O
  Put_Line("Digite seu nome:");
  Get_Line(nome);
  Put_Line(nome);

  -- Expressões booleanas
  if x > 5 and y < 20 then
    Put_Line("Condição satisfeita");
  end if;

end Main;