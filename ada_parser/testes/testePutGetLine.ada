-- Teste: Operações de I/O (Put_Line e Get_Line)
procedure IOCompleto is
begin
  Put_Line("Digite seu nome:");
  Get_Line(nome);

  Put_Line("Digite sua idade:");
  Get_Line(idade);

  Put_Line(nome);
  Put_Line(idade);

  Put_Line("Cadastro concluido");
end IOCompleto;