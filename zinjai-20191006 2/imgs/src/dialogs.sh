BUTTONS="button_ok button_cancel button_help button_find button_replace button_next button_prev button_stop button_reload"
for a in $BUTTONS; do ./export_one.sh dialogs.svg ../dialogs/b16 $a 16 16; done
for a in $BUTTONS; do ./export_one.sh dialogs.svg ../dialogs/b24 $a 24 24; done

ICONS="icon_error icon_info icon_warning icon_question"
for a in $ICONS; do ./export_one.sh dialogs.svg ../dialogs/i60 $a 60 60; done
for a in $ICONS; do ./export_one.sh dialogs.svg ../dialogs/i90 $a 90 90; done

PREFS="pref_help pref_general pref_debug pref_program pref_writing pref_skin pref_toolbars pref_paths pref_style"
for a in $PREFS; do ./export_one.sh dialogs.svg ../dialogs/p32 $a 32 32; done
for a in $PREFS; do ./export_one.sh dialogs.svg ../dialogs/p48 $a 48 48; done

HELP="help_prev help_next help_index help_copy help_tree help_print help_search help_atop"
for a in $HELP; do ./export_one.sh dialogs.svg ../dialogs/h24 $a 24 24; done
for a in $HELP; do ./export_one.sh dialogs.svg ../dialogs/h32 $a 32 32; done

./export_one.sh dialogs.svg ../dialogs new_wizard 141 250
./export_one.sh dialogs.svg ../dialogs big_tip 125 175
./export_one.sh dialogs.svg ../dialogs upgrade 70 130
