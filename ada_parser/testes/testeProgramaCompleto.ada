-- Teste 14: Programa completo com todas as funcionalidades
procedure ProgramaCompleto is
begin
  -- Inicialização
  Put_Line("=== Sistema de Notas ===");
  contador := 0;
  soma := 0;

  -- Input
  Put_Line("Digite o numero de alunos:");
  Get_Line(total);

  -- Processamento com loop
  while contador < total loop
    Put_Line("Digite a nota:");
    Get_Line(nota);

    -- Validação
    if nota >= 0 and nota <= 100 then
      soma := soma + nota;
      contador := contador + 1;

      -- Classificação
      if nota >= 90 then
        Put_Line("Excelente");
      else
        if nota >= 70 then
          Put_Line("Bom");
        else
          if nota >= 50 then
            Put_Line("Regular");
          else
            Put_Line("Insuficiente");
          end if;
        end if;
      end if;
    else
      Put_Line("Nota invalida");
    end if;
  end loop;

  -- Cálculo da média
  media := soma / total;
  Put_Line("Media da turma:");
  Put_Line(media);

  Put_Line("Processamento concluido");
end ProgramaCompleto;