-- Teste de while simples
procedure WhileSimples is
begin
  contador := 0;
  while contador < 5 loop
    Put_Line("Iteracao");
    contador := contador + 1;
  end loop;
end WhileSimples;