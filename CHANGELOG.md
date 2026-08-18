# Changelog

## 1.0.2 — 2026-08-18

### Corrigido / robustez de saída (relato: RG40XX-H / muOS não fechava)
- **Saída SELECT+START mais robusta em controles virtuais (muOS/gptokeyb).** Além
  do caminho por `GameController`, o chord agora lê também os **botões crus do
  joystick** nos mesmos índices que o mapping diz serem back/start — cobre CFWs em
  que a camada de GameController do controle virtual ("muOS-Keys") não propaga
  BACK/START. Tempo de hold reduzido para ~0,75 s.
- **Chord por evdev passa a vigiar DOIS combos em paralelo** — o derivado do bind
  SDL **e** o literal (`BTN_SELECT/BTN_START`/TRIGGER_HAPPY) — excluindo os códigos
  de L2/R2 do device (via bind de trigger) para nunca reintroduzir o "L2+R2 sai".
- **Handler de sinal (SIGTERM/SIGINT/SIGHUP) no binário:** se a frontend fechar o
  port matando o processo, ele encerra limpo com `_exit(0)` (o save já está no
  disco), mesmo que o laço principal esteja preso.

## 1.0.1 — 2026-08-17

### Corrigido
- **Apagar save agora funciona de verdade** — repetível e sem travar/fechar o jogo.
  O Forager confirma o apagar por `show_question_async`, que no Android chama o
  método nativo `RunnerJNILib.ShowQuestionAsync`; esse método faltava no loader,
  então a confirmação nunca respondia e o slot de save travava (apertar X "bipava"
  mas não apagava, e depois o A não entrava mais no jogo).
  - Implementados `ShowQuestion`, `ShowQuestionAsync`, `ShowMessage` e
    `ShowMessageAsync` no `RunnerJNILib`.
  - A resposta do diálogo é entregue pela via oficial do runtime
    (`InputQuery::SetResult`), **adiada para fora do `Kick()`** do laço principal
    para evitar reentrância/use-after-free, e com a string de resultado alocada
    no heap (o runtime faz `free()` nela) — isso corrige tanto o crash
    (`corrupted size vs prev_size`) quanto o travamento que aparecia a partir do
    **segundo** apagar na mesma sessão.
- **Exit chord SELECT+START** derivado diretamente do bind do SDL, corrigindo
  aparelhos em que o jogo saía com **L2+R2** em vez de SELECT+START.

### Mudado
- **Pasta do port renomeada de `forager` para `nxforager`**, para conviver lado a
  lado com instalações do PortMaster sem conflito de pasta. Migração para o formato
  de release "definitivo" (launcher/manifesto/estrutura de pacote).
- Guia de instalação (`INSTALLATION.md`) incluído no pacote de release.

### Notas
- Continua exigindo no máximo GLIBC 2.28; sem RPATH/RUNPATH e sem dados
  proprietários no pacote. O `port-env.sh` não cria swap.

## 1.0.0 — 2026-08-13

- First private universal PortMaster release.
- Preserves the Android GameMaker startup and runtime flow through a reproducible
  ARMv7 gmloader-next adapter.
- Adds transactional NXExtract 1.2.6 owner-package discovery for loose APK and
  base APK inside APKM/APKS/XAPK containers.
- Validates the exact 24 consumed owner members and emits one deterministic
  STORE-only runtime archive.
- Provides GLES2 video, music and sound effects, native SDL controllers, saves,
  clean Select+Start exit and twelve persistent language choices.
- Requires at most GLIBC 2.28; no RPATH/RUNPATH or proprietary data is shipped.
