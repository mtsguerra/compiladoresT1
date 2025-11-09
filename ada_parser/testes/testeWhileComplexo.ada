-- Teste de um while aninhado
procedure WhileAninhado is
begin
  i := 0;
  while i < 3 loop
    j := 0;
    while j < 3 loop
      Put_Line("Loop interno");
      j := j + 1;
    end loop;
    i := i + 1;
  end loop;
end WhileAninhado;