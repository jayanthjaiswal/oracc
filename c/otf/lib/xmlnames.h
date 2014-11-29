#ifndef _XMLNAMES_H
#define _XMLNAMES_H
#define MAX_A_NAME  20
#define MAX_QA_NAME 63
struct xname
{
  char pname[MAX_A_NAME];
  char qname[MAX_QA_NAME];
};
enum a_type
{
  n_c,
  n_g,
  n_n,
  n_note,
  n_syn,
  n_x,
  n_xh,
  n_xml,
  n_xtr,
  a_alt,
  a_base,
  a_class,
  a_cols,
  a_contrefs,
  a_endflag,
  a_ex_label,
  a_extent,
  a_flags,
  a_form,
  a_fragid,
  a_from,
  a_g_accented,
  a_g_break,
  a_g_breakEnd,
  a_g_breakStart,
  a_g_c,
  a_g_collated,
  a_g_damageEnd,
  a_g_damageStart,
  a_g_delim,
  a_g_em,
  a_g_hc,
  a_g_ho,
  a_g_logolang,
  a_g_o,
  a_g_pos,
  a_g_prox,
  a_g_queried,
  a_g_remarked,
  a_g_role,
  a_g_rws,
  a_g_script,
  a_g_sign,
  a_g_status,
  a_g_statusEnd,
  a_g_statusStart,
  a_g_surroEnd,
  a_g_surroStart,
  a_g_type,
  a_g_utf8,
  a_g_varc,
  a_g_vari,
  a_g_varo,
  a_guide,
  a_hand,
  a_haslinks,
  a_headform,
  a_headref,
  a_hlid,
  a_implicit,
  a_l,
  a_label,
  a_lang,
  a_lemma,
  a_level,
  a_lid,
  a_morph,
  a_n,
  a_n_num,
  a_norm,
  a_note_auto,
  a_note_label,
  a_note_mark,
  a_note_ref,
  a_o,
  a_p,
  a_place,
  a_plid,
  a_pos,
  a_primes,
  a_project,
  a_ref,
  a_scid,
  a_scope,
  a_score_mode,
  a_score_type,
  a_score_word,
  a_sense,
  a_sexified,
  a_sigref,
  a_silent,
  a_span,
  a_spanall,
  a_state,
  a_strict,
  a_subtype,
  a_swc_final,
  a_syn_brk_after,
  a_syn_brk_before,
  a_syn_ub_after,
  a_syn_ub_before,
  a_targ_id,
  a_targ_n,
  a_tid,
  a_to,
  a_type,
  a_unit,
  a_varnum,
  a_xml_id,
  a_xml_lang,
  a_xtr_cid,
  a_xtr_code,
  a_xtr_cols,
  a_xtr_disamb,
  a_xtr_eref,
  a_xtr_form,
  a_xtr_hdr_ref,
  a_xtr_lab_end_label,
  a_xtr_lab_end_lnum,
  a_xtr_lab_start_label,
  a_xtr_lab_start_lnum,
  a_xtr_label,
  a_xtr_lem,
  a_xtr_nrefs,
  a_xtr_overlap,
  a_xtr_ref,
  a_xtr_refs,
  a_xtr_rend_label,
  a_xtr_rows,
  a_xtr_se_label,
  a_xtr_silent,
  a_xtr_span,
  a_xtr_spanall,
  a_xtr_sref,
  a_xtr_standalone,
  a_xtr_type,
  a_xtr_unit,
  a_xtr_uref,
  a_enum_top
};
struct attr
{
  char *valpair[2];
  char *renpair[2];
};
enum e_type
{
  e_ag,
  e_atf,
  e_c,
  e_c_bib,
  e_c_bibd,
  e_c_bibliography,
  e_c_biby,
  e_c_catalog,
  e_c_cdf,
  e_c_document,
  e_c_field,
  e_c_gdl,
  e_c_key,
  e_c_p,
  e_c_record,
  e_c_section,
  e_c_val,
  e_cmt,
  e_column,
  e_composite,
  e_div,
  e_e,
  e_eg,
  e_f,
  e_g_a,
  e_g_b,
  e_g_c,
  e_g_d,
  e_g_f,
  e_g_g,
  e_g_gg,
  e_g_gloss,
  e_g_m,
  e_g_n,
  e_g_nonw,
  e_g_o,
  e_g_p,
  e_g_q,
  e_g_r,
  e_g_s,
  e_g_surro,
  e_g_swc,
  e_g_v,
  e_g_w,
  e_g_x,
  e_h,
  e_include,
  e_l,
  e_lg,
  e_m,
  e_n_g,
  e_n_grouped_word,
  e_n_s,
  e_n_w,
  e_n_word_group,
  e_nong,
  e_nonl,
  e_nonx,
  e_note_link,
  e_note_text,
  e_object,
  e_protocol,
  e_protocols,
  e_referto,
  e_score,
  e_sealing,
  e_sigdef,
  e_surface,
  e_surro,
  e_synopticon,
  e_transliteration,
  e_v,
  e_variant,
  e_variants,
  e_xh_div,
  e_xh_h1,
  e_xh_h2,
  e_xh_h3,
  e_xh_innerp,
  e_xh_p,
  e_xh_span,
  e_xtf,
  e_xtr_l2t,
  e_xtr_map,
  e_xtr_translation,
  e_enum_top
};

struct nstab { enum a_type prefix; const char *ns; };

extern struct attr abases[];
extern struct nstab nstab[];
extern struct xname anames[];
extern struct xname enames[];

#endif /*_XMLNAMES_H*/
