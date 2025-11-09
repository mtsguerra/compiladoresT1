-- Teste 15: Teste de stress com múltiplas operações
procedure StressTest is
begin
  -- Múltiplas variáveis
  a := 1;
  b := 2;
  c := 3;
  d := 4;
  e := 5;
  f := 6;
  g := 7;
  h := 8;
  i := 9;
  j := 10;

  -- Expressões complexas aninhadas
  resultado := ((a + b) * (c - d)) / ((e + f) - (g * h));

  -- Condições complexas
  if a < b and c > d or e = f and not g > h then
    Put_Line("Condicao 1");
  end if;

  -- Loops aninhados com condições
  x := 0;
  while x < 5 loop
    y := 0;
    while y < 5 loop
      if x = y then
        Put_Line("Diagonal");
      else
        if x > y then
          Put_Line("Acima");
        else
          Put_Line("Abaixo");
        end if;
      end if;
      y := y + 1;
    end loop;
    x := x + 1;
  end loop;

  -- Múltiplas operações de I/O
  Put_Line("Teste 1");
  Put_Line("Teste 2");
  Put_Line("Teste 3");
  Get_Line(input1);
  Get_Line(input2);
  Get_Line(input3);
  Put_Line(input1);
  Put_Line(input2);
  Put_Line(input3);

  Put_Line("Stress test concluido");
end StressTest;