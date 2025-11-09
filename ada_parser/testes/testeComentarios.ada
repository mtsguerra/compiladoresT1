-- Teste: Comentários em diferentes posições
procedure Comentarios is
begin
  -- Comentário no início
  x := 10; -- Comentário no meio da linha

  -- Múltiplas linhas de comentários
  -- Linha 1
  -- Linha 2
  -- Linha 3

  y := 20;

  -- Comentário antes do if
  if x < y then
    -- Comentário dentro do if
    Put_Line("X menor que Y");
  end if; -- Comentário após end if

  -- Comentário final
end Comentarios;