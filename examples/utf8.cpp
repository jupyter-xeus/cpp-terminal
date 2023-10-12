/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include <cpp-terminal/color.hpp>
#include <cpp-terminal/iostream.hpp>
#include <cpp-terminal/terminal.hpp>
#include <iostream>

int main()
{
  if(Term::terminal.supportUTF8()) std::cout << Term::color_fg(Term::Color::Name::Green) << "Your terminal support utf8 !\n\n" << Term::color_fg(Term::Color::Name::Default) << std::endl;
  else
    std::cout << Term::color_fg(Term::Color::Name::Red) << "Your terminal does NOT support utf8 !\n\n" << Term::color_fg(Term::Color::Name::Default) << std::endl;
  // clang-format off
  Term::cout
  << "UTF-8 encoded sample plain-text file\n"
  << "‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n"
  << "\n"
  << "Markus Kuhn [ˈmaʳkʊs kuːn] — 2002-07-25 CC BY\n"
  << "The ASCII compatible UTF-8 encoding used in this plain-text file is defined in Unicode, ISO 10646-1, and RFC 2279.\n"
  << "Using Unicode/UTF-8, you can write in emails and source code things such as\n"
  << "\n"
  << Term::color_fg(Term::Color::Name::Green)
  << "Mathematics and sciences:\n"
  << Term::color_fg(Term::Color::Name::Default)
  << "\n"
  << "∮ E⋅da = Q,  n → ∞, ∑ f(i) = ∏ g(i),    ⎧⎡⎛┌─────┐⎞⎤⎫\n"
  << "                                        ⎪⎢⎜│a²+b³ ⎟⎥⎪\n"
  << "∀x∈ℝ: ⌈x⌉ = −⌊−x⌋, α ∧ ¬β = ¬(¬α ∨ β),  ⎪⎢⎜│───── ⎟⎥⎪\n"
  << "                                        ⎪⎢⎜⎷ c₈   ⎟⎥⎪\n"
  << "ℕ ⊆ ℕ₀ ⊂ ℤ ⊂ ℚ ⊂ ℝ ⊂ ℂ,                 ⎨⎢⎜       ⎟⎥⎬\n"
  << "                                        ⎪⎢⎜ ∞     ⎟⎥⎪\n"
  << "⊥ < a ≠ b ≡ c ≤ d ≪ ⊤ ⇒ (⟦A⟧ ⇔ ⟪B⟫),    ⎪⎢⎜ ⎲     ⎟⎥⎪\n"
  << "                                        ⎪⎢⎜ ⎳aⁱ-bⁱ⎟⎥⎪\n"
  << "2H₂ + O₂ ⇌ 2H₂O, R = 4.7 kΩ, ⌀ 200 mm   ⎩⎣⎝i=1    ⎠⎦⎭\n"
  << "\n"
  << Term::color_fg(Term::Color::Name::Green)
  << "Linguistics and dictionaries:\n"
  << Term::color_fg(Term::Color::Name::Default)
  << "\n"
  << "ði ıntəˈnæʃənəl fəˈnɛtık əsoʊsiˈeıʃn\n"
  << "Y [ˈʏpsilɔn], Yen [jɛn], Yoga [ˈjoːgɑ]\n"
  << "APL:\n"
  << "\n"
  << "((V⍳V)=⍳⍴V)/V←,V    ⌷←⍳→⍴∆∇⊃‾⍎⍕⌈\n"
  << "\n"
  << Term::color_fg(Term::Color::Name::Green)
  << "Nicer typography in plain text files:\n"
  << Term::color_fg(Term::Color::Name::Default)
  << "\n"
  << "╔══════════════════════════════════════════╗\n"
  << "║                                          ║\n"
  << "║   • ‘single’ and “double” quotes         ║\n"
  << "║                                          ║\n"
  << "║   • Curly apostrophes: “We’ve been here” ║\n"
  << "║                                          ║\n"
  << "║   • Latin-1 apostrophe and accents: '´`  ║\n"
  << "║                                          ║\n"
  << "║   • ‚deutsche‘ „Anführungszeichen“       ║\n"
  << "║                                          ║\n"
  << "║   • †, ‡, ‰, •, 3–4, —, −5/+5, ™, …      ║\n"
  << "║                                          ║\n"
  << "║   • ASCII safety test: 1lI|, 0OD, 8B     ║\n"
  << "║                      ╭─────────╮         ║\n"
  << "║   • the euro symbol: │ 14.95 € │         ║\n"
  << "║                      ╰─────────╯         ║\n"
  << "╚══════════════════════════════════════════╝\n"
  << "\n"
  << Term::color_fg(Term::Color::Name::Green)
  << "Combining characters:\n"
  << Term::color_fg(Term::Color::Name::Default)
  << "\n"
  << "STARGΛ̊TE SG-1, a = v̇ = r̈, a⃑ ⊥ b⃑\n"
  << "\n"
  << Term::color_fg(Term::Color::Name::Green)
  << "Greek (in Polytonic):\n"
  << Term::color_fg(Term::Color::Name::Default)
  << "\n"
  << "The Greek anthem:\n"
  << "\n"
  << "Σὲ γνωρίζω ἀπὸ τὴν κόψη\n"
  << "τοῦ σπαθιοῦ τὴν τρομερή,\n"
  << "σὲ γνωρίζω ἀπὸ τὴν ὄψη\n"
  << "ποὺ μὲ βία μετράει τὴ γῆ.\n"
  << "\n"
  << "᾿Απ᾿ τὰ κόκκαλα βγαλμένη\n"
  << "τῶν ῾Ελλήνων τὰ ἱερά\n"
  << "καὶ σὰν πρῶτα ἀνδρειωμένη\n"
  << "χαῖρε, ὦ χαῖρε, ᾿Ελευθεριά!\n"
  << "\n"
  << "From a speech of Demosthenes in the 4th century BC:\n"
  << "\n"
  << "Οὐχὶ ταὐτὰ παρίσταταί μοι γιγνώσκειν, ὦ ἄνδρες ᾿Αθηναῖοι,\n"
  << "ὅταν τ᾿ εἰς τὰ πράγματα ἀποβλέψω καὶ ὅταν πρὸς τοὺς\n"
  << "λόγους οὓς ἀκούω· τοὺς μὲν γὰρ λόγους περὶ τοῦ\n"
  << "τιμωρήσασθαι Φίλιππον ὁρῶ γιγνομένους, τὰ δὲ πράγματ᾿\n"
  << "εἰς τοῦτο προήκοντα,  ὥσθ᾿ ὅπως μὴ πεισόμεθ᾿ αὐτοὶ\n"
  << "πρότερον κακῶς σκέψασθαι δέον. οὐδέν οὖν ἄλλο μοι δοκοῦσιν\n"
  << "οἱ τὰ τοιαῦτα λέγοντες ἢ τὴν ὑπόθεσιν, περὶ ἧς βουλεύεσθαι,\n"
  << "οὐχὶ τὴν οὖσαν παριστάντες ὑμῖν ἁμαρτάνειν. ἐγὼ δέ, ὅτι μέν\n"
  << "ποτ᾿ ἐξῆν τῇ πόλει καὶ τὰ αὑτῆς ἔχειν ἀσφαλῶς καὶ Φίλιππον\n"
  << "τιμωρήσασθαι, καὶ μάλ᾿ ἀκριβῶς οἶδα· ἐπ᾿ ἐμοῦ γάρ, οὐ πάλαι\n"
  << "γέγονεν ταῦτ᾿ ἀμφότερα· νῦν μέντοι πέπεισμαι τοῦθ᾿ ἱκανὸν\n"
  << "προλαβεῖν ἡμῖν εἶναι τὴν πρώτην, ὅπως τοὺς συμμάχους\n"
  << "σώσομεν. ἐὰν γὰρ τοῦτο βεβαίως ὑπάρξῃ, τότε καὶ περὶ τοῦ\n"
  << "τίνα τιμωρήσεταί τις καὶ ὃν τρόπον ἐξέσται σκοπεῖν· πρὶν δὲ\n"
  << "τὴν ἀρχὴν ὀρθῶς ὑποθέσθαι, μάταιον ἡγοῦμαι περὶ τῆς\n"
  << "τελευτῆς ὁντινοῦν ποιεῖσθαι λόγον.\n"
  << "\n"
  << "Δημοσθένους, Γ´ ᾿Ολυνθιακὸς\n"
  << "\n"
  << Term::color_fg(Term::Color::Name::Green)
  << "Georgian:\n"
  << Term::color_fg(Term::Color::Name::Default)
  << "\n"
  << "From a Unicode conference invitation:\n"
  << "\n"
  << "გთხოვთ ახლავე გაიაროთ რეგისტრაცია Unicode-ის მეათე საერთაშორისო\n"
  << "კონფერენციაზე დასასწრებად, რომელიც გაიმართება 10-12 მარტს,\n"
  << "ქ. მაინცში, გერმანიაში. კონფერენცია შეჰკრებს ერთად მსოფლიოს\n"
  << "ექსპერტებს ისეთ დარგებში როგორიცაა ინტერნეტი და Unicode-ი,\n"
  << "ინტერნაციონალიზაცია და ლოკალიზაცია, Unicode-ის გამოყენება\n"
  << "ოპერაციულ სისტემებსა, და გამოყენებით პროგრამებში, შრიფტებში,\n"
  << "ტექსტების დამუშავებასა და მრავალენოვან კომპიუტერულ სისტემებში.\n"
  << "\n"
  << Term::color_fg(Term::Color::Name::Green)
  << "Russian:\n"
  << Term::color_fg(Term::Color::Name::Default)
  << "\n"
  << "From a Unicode conference invitation:\n"
  << "\n"
  << "Зарегистрируйтесь сейчас на Десятую Международную Конференцию по\n"
  << "Unicode, которая состоится 10-12 марта 1997 года в Майнце в Германии.\n"
  << "Конференция соберет широкий круг экспертов по  вопросам глобального\n"
  << "Интернета и Unicode, локализации и интернационализации, воплощению и\n"
  << "применению Unicode в различных операционных системах и программных\n"
  << "приложениях, шрифтах, верстке и многоязычных компьютерных системах.\n"
  << "\n"
  << Term::color_fg(Term::Color::Name::Green)
  << "Thai (UCS Level 2):\n"
  << Term::color_fg(Term::Color::Name::Default)
  << "\n"
  << "Excerpt from a poetry on The Romance of The Three Kingdoms (a Chinese classic 'San Gua'):\n"
  << "\n"
  << "[----------------------------|------------------------]\n"
  << "๏ แผ่นดินฮั่นเสื่อมโทรมแสนสังเวช  พระปกเกศกองบู๊กู้ขึ้นใหม่\n"
  << "สิบสองกษัตริย์ก่อนหน้าแลถัดไป       สององค์ไซร้โง่เขลาเบาปัญญา\n"
  << "ทรงนับถือขันทีเป็นที่พึ่ง           บ้านเมืองจึงวิปริตเป็นนักหนา\n"
  << "โฮจิ๋นเรียกทัพทั่วหัวเมืองมา         หมายจะฆ่ามดชั่วตัวสำคัญ\n"
  << "เหมือนขับไสไล่เสือจากเคหา      รับหมาป่าเข้ามาเลยอาสัญ\n"
  << "ฝ่ายอ้องอุ้นยุแยกให้แตกกัน          ใช้สาวนั้นเป็นชนวนชื่นชวนใจ\n"
  << "พลันลิฉุยกุยกีกลับก่อเหตุ          ช่างอาเพศจริงหนาฟ้าร้องไห้\n"
  << "ต้องรบราฆ่าฟันจนบรรลัย           ฤๅหาใครค้ำชูกู้บรรลังก์ ฯ\n"
  << "\n"
  << "(The above is a two-column text. If combining characters are handled correctly, the lines of the second column should be aligned with the | character above.)\n"
  << "\n"
  << Term::color_fg(Term::Color::Name::Green)
  << "Ethiopian:\n"
  << Term::color_fg(Term::Color::Name::Default)
  << "\n"
  << "Proverbs in the Amharic language:\n"
  << "\n"
  << "ሰማይ አይታረስ ንጉሥ አይከሰስ።\n"
  << "ብላ ካለኝ እንደአባቴ በቆመጠኝ።\n"
  << "ጌጥ ያለቤቱ ቁምጥና ነው።\n"
  << "ደሀ በሕልሙ ቅቤ ባይጠጣ ንጣት በገደለው።\n"
  << "የአፍ ወለምታ በቅቤ አይታሽም።\n"
  << "አይጥ በበላ ዳዋ ተመታ።\n"
  << "ሲተረጉሙ ይደረግሙ።\n"
  << "ቀስ በቀስ፥ ዕንቁላል በእግሩ ይሄዳል።\n"
  << "ድር ቢያብር አንበሳ ያስር።\n"
  << "ሰው እንደቤቱ እንጅ እንደ ጉረቤቱ አይተዳደርም።\n"
  << "እግዜር የከፈተውን ጉሮሮ ሳይዘጋው አይድርም።\n"
  << "የጎረቤት ሌባ፥ ቢያዩት ይስቅ ባያዩት ያጠልቅ።\n"
  << "ሥራ ከመፍታት ልጄን ላፋታት።\n"
  << "ዓባይ ማደሪያ የለው፥ ግንድ ይዞ ይዞራል።\n"
  << "የእስላም አገሩ መካ የአሞራ አገሩ ዋርካ።\n"
  << "ተንጋሎ ቢተፉ ተመልሶ ባፉ።\n"
  << "ወዳጅህ ማር ቢሆን ጨርስህ አትላሰው።\n"
  << "እግርህን በፍራሽህ ልክ ዘርጋ።\n"
  << "\n"
  << Term::color_fg(Term::Color::Name::Green)
  << "Runes:\n"
  << Term::color_fg(Term::Color::Name::Default)
  << "\n"
  << "ᚻᛖ ᚳᚹᚫᚦ ᚦᚫᛏ ᚻᛖ ᛒᚢᛞᛖ ᚩᚾ ᚦᚫᛗ ᛚᚪᚾᛞᛖ ᚾᚩᚱᚦᚹᛖᚪᚱᛞᚢᛗ ᚹᛁᚦ ᚦᚪ ᚹᛖᛥᚫ\n"
  << "\n"
  << "(Old English, which transcribed into Latin reads 'He cwaeth that he bude thaem lande northweardum with tha Westsae.' and means 'He said that he lived in the northern land near the Western Sea.')\n"
  << "\n"
  << Term::color_fg(Term::Color::Name::Green)
  << "Braille:\n"
  << Term::color_fg(Term::Color::Name::Default)
  << "\n"
  << "⡌⠁⠧⠑ ⠼⠁⠒  ⡍⠜⠇⠑⠹⠰⠎ ⡣⠕⠌\n"
  << "\n"
  << "⡍⠜⠇⠑⠹ ⠺⠁⠎ ⠙⠑⠁⠙⠒ ⠞⠕ ⠃⠑⠛⠔ ⠺⠊⠹⠲ ⡹⠻⠑ ⠊⠎ ⠝⠕ ⠙⠳⠃⠞\n"
  << "⠱⠁⠞⠑⠧⠻ ⠁⠃⠳⠞ ⠹⠁⠞⠲ ⡹⠑ ⠗⠑⠛⠊⠌⠻ ⠕⠋ ⠙⠊⠎ ⠃⠥⠗⠊⠁⠇ ⠺⠁⠎\n"
  << "⠎⠊⠛⠝⠫ ⠃⠹ ⠹⠑ ⠊⠇⠻⠛⠹⠍⠁⠝⠂ ⠹⠑ ⠊⠇⠻⠅⠂ ⠹⠑ ⠥⠝⠙⠻⠞⠁⠅⠻⠂\n"
  << "⠁⠝⠙ ⠹⠑ ⠡⠊⠑⠋ ⠍⠳⠗⠝⠻⠲ ⡎⠊⠗⠕⠕⠛⠑ ⠎⠊⠛⠝⠫ ⠊⠞⠲ ⡁⠝⠙\n"
  << "⡎⠊⠗⠕⠕⠛⠑⠰⠎ ⠝⠁⠍⠑ ⠺⠁⠎ ⠛⠕⠕⠙ ⠥⠏⠕⠝ ⠰⡡⠁⠝⠛⠑⠂ ⠋⠕⠗ ⠁⠝⠹⠹⠔⠛ ⠙⠑\n"
  << "⠡⠕⠎⠑ ⠞⠕ ⠏⠥⠞ ⠙⠊⠎ ⠙⠁⠝⠙ ⠞⠕⠲\n"
  << "\n"
  << "⡕⠇⠙ ⡍⠜⠇⠑⠹ ⠺⠁⠎ ⠁⠎ ⠙⠑⠁⠙ ⠁⠎ ⠁ ⠙⠕⠕⠗⠤⠝⠁⠊⠇⠲\n"
  << "\n"
  << "⡍⠔⠙⠖ ⡊ ⠙⠕⠝⠰⠞ ⠍⠑⠁⠝ ⠞⠕ ⠎⠁⠹ ⠹⠁⠞ ⡊ ⠅⠝⠪⠂ ⠕⠋ ⠍⠹\n"
  << "⠪⠝ ⠅⠝⠪⠇⠫⠛⠑⠂ ⠱⠁⠞ ⠹⠻⠑ ⠊⠎ ⠏⠜⠞⠊⠊⠥⠇⠜⠇⠹ ⠙⠑⠁⠙ ⠁⠃⠳⠞\n"
  << "⠁ ⠙⠕⠕⠗⠤⠝⠁⠊⠇⠲ ⡊ ⠍⠊⠣⠞ ⠙⠁⠧⠑ ⠃⠑⠲ ⠔⠊⠇⠔⠫⠂ ⠍⠹⠎⠑⠇⠋⠂ ⠞⠕\n"
  << "⠗⠑⠛⠜⠙ ⠁ ⠊⠕⠋⠋⠔⠤⠝⠁⠊⠇ ⠁⠎ ⠹⠑ ⠙⠑⠁⠙⠑⠌ ⠏⠊⠑⠊⠑ ⠕⠋ ⠊⠗⠕⠝⠍⠕⠝⠛⠻⠹\n"
  << "⠔ ⠹⠑ ⠞⠗⠁⠙⠑⠲ ⡃⠥⠞ ⠹⠑ ⠺⠊⠎⠙⠕⠍ ⠕⠋ ⠳⠗ ⠁⠝⠊⠑⠌⠕⠗⠎\n"
  << "⠊⠎ ⠔ ⠹⠑ ⠎⠊⠍⠊⠇⠑⠆ ⠁⠝⠙ ⠍⠹ ⠥⠝⠙⠁⠇⠇⠪⠫ ⠙⠁⠝⠙⠎\n"
  << "⠩⠁⠇⠇ ⠝⠕⠞ ⠙⠊⠌⠥⠗⠃ ⠊⠞⠂ ⠕⠗ ⠹⠑ ⡊⠳⠝⠞⠗⠹⠰⠎ ⠙⠕⠝⠑ ⠋⠕⠗⠲ ⡹⠳\n"
  << "⠺⠊⠇⠇ ⠹⠻⠑⠋⠕⠗⠑ ⠏⠻⠍⠊⠞ ⠍⠑ ⠞⠕ ⠗⠑⠏⠑⠁⠞⠂ ⠑⠍⠏⠙⠁⠞⠊⠊⠁⠇⠇⠹⠂ ⠹⠁⠞\n"
  << "⡍⠜⠇⠑⠹ ⠺⠁⠎ ⠁⠎ ⠙⠑⠁⠙ ⠁⠎ ⠁ ⠙⠕⠕⠗⠤⠝⠁⠊⠇⠲\n"
  << "\n"
  << "(The first couple of paragraphs of \"A Christmas Carol\" by Dickens)\n"
  << "\n"
  << Term::color_fg(Term::Color::Name::Green)
  << "Compact font selection example text:\n"
  << Term::color_fg(Term::Color::Name::Default)
  << "\n"
  << "ABCDEFGHIJKLMNOPQRSTUVWXYZ /0123456789\n"
  << "abcdefghijklmnopqrstuvwxyz £©µÀÆÖÞßéöÿ\n"
  << "–—‘“”„†•…‰™œŠŸž€ ΑΒΓΔΩαβγδω АБВГДабвгд\n"
  << "∀∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂӥẄɐː⍎אԱა\n"
  << "\n"
  << Term::color_fg(Term::Color::Name::Green)
  << "Greetings in various languages:\n"
  << Term::color_fg(Term::Color::Name::Default)
  << "\n"
  << "Hello world, Καλημέρα κόσμε, コンニチハ\n"
  << "\n"
  << Term::color_fg(Term::Color::Name::Green)
  << "Box drawing alignment tests:\n"
  << Term::color_fg(Term::Color::Name::Default)
  << "                                                                    █\n"
  << "╔══╦══╗  ┌──┬──┐  ╭──┬──╮  ╭──┬──╮  ┏━━┳━━┓  ┎┒┏┑   ╷  ╻ ┏┯┓ ┌┰┐    ▊ ╱╲╱╲╳╳╳\n"
  << "║┌─╨─┐║  │╔═╧═╗│  │╒═╪═╕│  │╓─╁─╖│  ┃┌─╂─┐┃  ┗╃╄┙  ╶┼╴╺╋╸┠┼┨ ┝╋┥    ▋ ╲╱╲╱╳╳╳\n"
  << "║│╲ ╱│║  │║   ║│  ││ │ ││  │║ ┃ ║│  ┃│ ╿ │┃  ┍╅╆┓   ╵  ╹ ┗┷┛ └┸┘    ▌ ╱╲╱╲╳╳╳\n"
  << "╠╡ ╳ ╞╣  ├╢   ╟┤  ├┼─┼─┼┤  ├╫─╂─╫┤  ┣┿╾┼╼┿┫  ┕┛┖┚     ┌┄┄┐ ╎ ┏┅┅┓ ┋ ▍ ╲╱╲╱╳╳╳\n"
  << "║│╱ ╲│║  │║   ║│  ││ │ ││  │║ ┃ ║│  ┃│ ╽ │┃  ░░▒▒▓▓██ ┊  ┆ ╎ ╏  ┇ ┋ ▎\n"
  << "║└─╥─┘║  │╚═╤═╝│  │╘═╪═╛│  │╙─╀─╜│  ┃└─╂─┘┃  ░░▒▒▓▓██ ┊  ┆ ╎ ╏  ┇ ┋ ▏\n"
  << "╚══╩══╝  └──┴──┘  ╰──┴──╯  ╰──┴──╯  ┗━━┻━━┛  ▗▄▖▛▀▜   └╌╌┘ ╎ ┗╍╍┛ ┋  ▁▂▃▄▅▆▇█\n"
  << "                                             ▝▀▘▙▄▟\n"<<std::flush;
  // clang-format on
}
