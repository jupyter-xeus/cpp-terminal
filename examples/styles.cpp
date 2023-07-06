
#include <cpp-terminal/color.hpp>
#include <cpp-terminal/style.hpp>
#include <cpp-terminal/terminal.hpp>

int main()
{
  Term::terminal << "Normal Text " << Term::Style::BOLD << "BOLD" << Term::Style::RESET_BOLD_DIM << std::endl;
  Term::terminal << "Normal Text " << Term::Style::DIM << "DIM" << Term::Style::RESET_BOLD_DIM << std::endl;
  Term::terminal << "Normal Text " << Term::Style::ITALIC << "ITALIC" << Term::Style::RESET_ITALIC << std::endl;
  Term::terminal << "Normal Text " << Term::Style::UNDERLINE << "UNDERLINE" << Term::Style::RESET_UNDERLINE << std::endl;
  Term::terminal << "Normal Text " << Term::Style::BLINK << "BLINK" << Term::Style::RESET_BLINKING << std::endl;
  Term::terminal << "Normal Text " << Term::Style::BLINK_RAPID << "BLINK_RAPID" << Term::Style::RESET_BLINKING << std::endl;
  Term::terminal << "Normal Text " << Term::Style::REVERSED << "REVERSED" << Term::Style::RESET_REVERSED << std::endl;
  Term::terminal << "Normal Text " << Term::Style::CONCEAL << "CONCEAL" << Term::Style::RESET_CONCEAL << std::endl;
  Term::terminal << "Normal Text " << Term::Style::CROSSED << "CROSSED" << Term::Style::RESET_CROSSED << std::endl;

  Term::terminal << "Normal Text " << Term::Style::FONT_0 << "FONT_0" << Term::Style::FONT_0 << std::endl;
  Term::terminal << "Normal Text " << Term::Style::FONT_1 << "FONT_1" << Term::Style::FONT_0 << std::endl;
  Term::terminal << "Normal Text " << Term::Style::FONT_2 << "FONT_2" << Term::Style::FONT_0 << std::endl;
  Term::terminal << "Normal Text " << Term::Style::FONT_3 << "FONT_3" << Term::Style::FONT_0 << std::endl;
  Term::terminal << "Normal Text " << Term::Style::FONT_4 << "FONT_4" << Term::Style::FONT_0 << std::endl;
  Term::terminal << "Normal Text " << Term::Style::FONT_5 << "FONT_5" << Term::Style::FONT_0 << std::endl;
  Term::terminal << "Normal Text " << Term::Style::FONT_6 << "FONT_6" << Term::Style::FONT_0 << std::endl;
  Term::terminal << "Normal Text " << Term::Style::FONT_7 << "FONT_7" << Term::Style::FONT_0 << std::endl;
  Term::terminal << "Normal Text " << Term::Style::FONT_8 << "FONT_8" << Term::Style::FONT_0 << std::endl;
  Term::terminal << "Normal Text " << Term::Style::FONT_9 << "FONT_9" << Term::Style::FONT_0 << std::endl;
  Term::terminal << "Normal Text " << Term::Style::FONT_10 << "FONT_10" << Term::Style::FONT_0 << std::endl;

  Term::terminal << "Normal Text " << Term::color_fg(255, 0, 0) << "colored" << Term::Style::DEFAULT_FOREGROUND_COLOR << " back to default" << std::endl;
  Term::terminal << "Normal Text " << Term::color_bg(0, 255, 0) << "colored" << Term::Style::DEFAULT_BACKGROUND_COLOR << " back to default" << std::endl;

  Term::terminal << "Normal Text " << Term::Style::FRAME << "FRAME" << Term::Style::RESET_FRAME_ENCIRCLE << std::endl;
  Term::terminal << "Normal Text " << Term::Style::ENCIRCLE << "ENCIRCLE" << Term::Style::RESET_FRAME_ENCIRCLE << std::endl;
  Term::terminal << "Normal Text " << Term::Style::OVERLINE << "OVERLINE" << Term::Style::RESET_OVERLINE << std::endl;

  Term::terminal << "Normal Text " << Term::Style::BAR_RIGHT << "BAR_RIGHT" << Term::Style::RESET_BAR << std::endl;
  Term::terminal << "Normal Text " << Term::Style::DOUBLE_BAR_RIGHT << "DOUBLE_BAR_RIGHT" << Term::Style::RESET_BAR << std::endl;
  Term::terminal << "Normal Text " << Term::Style::BAR_LEFT << "BAR_LEFT" << Term::Style::RESET_BAR << std::endl;
  Term::terminal << "Normal Text " << Term::Style::DOUBLE_BAR_LEFT << "DOUBLE_BAR_LEFT" << Term::Style::RESET_BAR << std::endl;
  Term::terminal << "Normal Text " << Term::Style::STRESS_MARKING << "STRESS_MARKING" << Term::Style::RESET_BAR << std::endl;

  Term::terminal << "Normal Text " << Term::Style::SUPERSCRIPT << "SUPERSCRIPT" << Term::Style::RESET_SUPERSCRIPT_SUBSCRIPT << std::endl;
  Term::terminal << "Normal Text " << Term::Style::SUBSCRIPT << "SUBSCRIPT" << Term::Style::RESET_SUPERSCRIPT_SUBSCRIPT << std::endl;
}
