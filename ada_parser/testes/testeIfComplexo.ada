-- Teste de um if aninhado
procedure IfAninhado is
begin
  nota := 85;
  if nota >= 90 then
    Put_Line("A");
  else
    if nota >= 80 then
      Put_Line("B");
    else
      if nota >= 70 then
        Put_Line("C");
      else
        Put_Line("D");
      end if;
    end if;
  end if;
end IfAninhado;