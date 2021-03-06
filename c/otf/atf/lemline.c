#include <ctype128.h>
#include "hash.h"
#include "warning.h"
#include "tree.h"
#include "memblock.h"
#include "lemline.h"
#include "ilem_form.h"
#include "xcl.h"
#include "sigs.h"
#include "xmd.h"
#include "inline.h"
#include "tokenizer.h"
#include "npool.h"
#include "xmlutil.h"
#include "lang.h"

struct serializer_frame 
{
  char *pre;
  char *lit;
  char *pst;
};

struct lem_save
{
  unsigned char *line;
  int lnum;
  int* cells; /* parallel with forms */
  struct ilem_form **forms;
  int forms_used;
  int forms_alloced;
  struct lem_save *cont;
};

Hash_table *word_form_index;
extern const char *curr_dialect;
static void set_sframe(struct serializer_frame *sfp, char *literal);

struct lem_save *lem_lines = NULL;
int lem_lines_used = 0;
int lem_lines_alloced = 0;
unsigned int lem_stop_lnum = UINT_MAX;

extern const char *project;
extern const char *textid;
extern int lem_standalone;
extern int verbose;
Hash_table *word_form_index;

static const char *peri, *prov,*genr,*subg;

struct xcl_context *lemline_xcp;

static struct lem_save*
new_lsp(void)
{
  struct lem_save *lsp;
  if (lem_lines_used == lem_lines_alloced)
    {
      lem_lines_alloced += 1024;
      lem_lines = realloc(lem_lines,
			   lem_lines_alloced * sizeof(struct lem_save));
    }
  lsp = &lem_lines[lem_lines_used++];
  lsp->forms_used = lsp->forms_alloced = 0;
  lsp->forms = NULL;
  lsp->cont = NULL;
  lsp->lnum = lnum;
  lsp->cells = NULL;
  return lsp;
}

static struct lem_save *curr_lsp, *last_lsp;

void
lemline_init(void)
{
  inline_functions(lem_save_form,lem_unform,lem_reset_form);
  lemline_xcp = calloc(1,sizeof(struct xcl_context));
  lemline_xcp->project = xcl_project = project;
  lemline_xcp->textid = textid;
  lemline_xcp->pool = npool_init();
  lemline_xcp->sigs = sig_context_init();
}

void
lemline_term(void)
{
  npool_term(lemline_xcp->pool);
#if 0
  sig_context_term(lemline_xcp->sigs);
#endif
  free(lemline_xcp);
  lemline_xcp = NULL;
}

void
lem_text_init(const char *pq)
{
  Hash_table *lem_xmd;
  xmd_init();
  lem_xmd = xmd_load(project, pq);
  peri = hash_find(lem_xmd,(unsigned char *)"period");
  prov = hash_find(lem_xmd,(unsigned char *)"provenience");
  genr = hash_find(lem_xmd,(unsigned char *)"genre");
  subg = hash_find(lem_xmd,(unsigned char *)"subgenre");
  /*  hash_free(lem_xmd, NULL); */ /* NO: xmd_init does this for itself */
  word_form_index = hash_create(200);
}

void
lem_text_term()
{
  if (word_form_index)
    hash_free(word_form_index, NULL);
  if (lem_lines)
    {
      int i;
      for (i = 0; i < lem_lines_used; ++i)
	{
	  free(lem_lines[i].forms);
	  free(lem_lines[i].cells);
	}
      lem_lines_used = lem_lines_alloced = 0;
      free(lem_lines);
      lem_lines = NULL;
    }
  xmd_term();
}

static const char *
lem_base_type(const char *form)
{
  if (isdigit(*form) || 
      (('n' == *form || 'N' == *form) && (form[1] == '\0' || form[1] == '(')))
    return "n";
  else if (strchr(form,'x') || strchr(form,'$') || !strcmp(form,"o"))
    return "u";
  else if (!form[1] && form[0] == '-')
    return "-";
  else
    return "X";
}

const char *
lem_meta_id(void)
{
  static char buf[16];
  sprintf(buf,"%s.m0",textid);
  return buf;
}

const char *const*
lem_text_meta(void)
{
  const char**m = malloc(10*sizeof(char*));
  m[0] = "period";
  m[1] = peri;
  m[2] = "provenience";
  m[3] = prov;
  m[4] = "genre";
  m[5] = genr;
  m[6] = "subgenre";
  m[7] = subg;
  m[8] = m[9] = NULL;
  return m;
}

void
lem_save_line(unsigned char *lp)
{
  last_lsp = curr_lsp = new_lsp();
  curr_lsp->line = npool_copy(lp,lemline_xcp->pool);
}

void
lem_save_cont(unsigned char *lp)
{
  struct lem_save *sp;
  for (sp = curr_lsp; sp->cont; sp = sp->cont)
    ;
  sp->cont = new_lsp();
  sp->cont->line = npool_copy(lp,lemline_xcp->pool);
}

#if 0
void
lem_append_line(unsigned char *lp)
{
  struct lem_save *sp;
  for (sp = curr_lsp; sp->next; sp = sp->next)
    ;
  last_lsp = sp->next = new_lsp();
  sp->next->line = npool_copy(lp,lemline_xcp->pool);
}
#endif

void
lem_reset_form(const char *ref, const char *form)
{
  struct ilem_form *fp = hash_find(word_form_index,(unsigned char *)ref);
  if (fp)
    {
      fp->f2.form = npool_copy((unsigned char *)form,lemline_xcp->pool);
      form = (const char*)fp->f2.form;
    }	
}

/* This routine should not set anything but FORM at the f2 level;
   that is the job of ilem_parse */
void
lem_save_form(const char *ref, const char *lang, 
	      const char *formstr, struct lang_context *langcon)
{
  struct ilem_form *form = mb_new(lemline_xcp->sigs->mb_ilem_forms);
  extern int curr_cell;
  form->ref = (char*)ref;
  if (lang)
    {
      form->f2.lang = (unsigned char*)lang;
      form->f2.core = langcore_of(lang);
      if (strstr(lang,"949"))
	  BIT_SET(form->f2.flags,F2_FLAGS_LEM_BY_NORM);
    }
  if (BIT_ISSET(form->f2.flags,F2_FLAGS_LEM_BY_NORM))
    {
      form->f2.norm = (unsigned char *)formstr;
      form->f2.form = (const unsigned char *)"*";
    }
  else
    form->f2.form = (unsigned char *)formstr;
  form->file = (char*)file;
  form->lnum = lnum;
  form->lang = langcon;

  if (!ref[0])
    return;

  if (!curr_lsp->forms_alloced
      || curr_lsp->forms_used == curr_lsp->forms_alloced)
    {
      curr_lsp->forms_alloced += 16;
      curr_lsp->forms = realloc(curr_lsp->forms,
				curr_lsp->forms_alloced*sizeof(struct ilem_form*));
      curr_lsp->cells = realloc(curr_lsp->cells,
				curr_lsp->forms_alloced*sizeof(int));
      if (curr_lsp->forms_used < 0)
	curr_lsp->forms_used = 0;
    }
  /* when curr_cell = 0 we are in a line with no cells; by definition,
     all content in such a line is in cell 2 (because cell 1 is the line
     number) */
  curr_lsp->cells[curr_lsp->forms_used] = (curr_cell ? curr_cell : 2);
  curr_lsp->forms[curr_lsp->forms_used++] = form;
  hash_add(word_form_index,npool_copy((unsigned char*)ref,lemline_xcp->pool),form);
}

void
lem_save_lemma(struct node *wp, const char *lemma)
{
  struct ilem_form *form = hash_find(word_form_index, getAttr(wp,"xml:id"));
  if (form)
    form->literal = (char*)npool_copy((unsigned char *)lemma,lemline_xcp->pool);
  else
    vwarning("internal error: word_form_index lookup failed");
}

void
lem_unform(void)
{
  if (curr_lsp)
    --curr_lsp->forms_used;
}

#define xuse_literal(xl) ((xl) && ((xl)[1] || ((xl)[0] != 'X' && (xl)[0] != 'L')))

#define use_literal(xl) (xl)

static void
lem_f2_serialize(FILE *fp, struct f2 *f2)
{
  if (BIT_ISSET(f2->flags, F2_FLAGS_NOT_IN_SIGS))
    {
      if (f2->pos)
	{
	  fputs((char*)f2->pos,fp);
	}
      else
	fputs("X",fp);
    }
  else
    {
      if (f2->norm || f2->cf)
	{
	  if (strncmp((char*)f2->lang,"sux",3) && f2->norm)
	    fputs((char*)f2->norm,fp);
	  else
	    fputs((char*)f2->cf,fp);
	  if (f2->sense)
	    fprintf(fp,"[%s]",(char*)f2->sense);
	  else
	    fprintf(fp,"[%s]",f2->gw);
	  if (f2->epos && strcmp((char*)f2->pos,(char*)f2->epos))
	    fprintf(fp,"'%s",f2->epos);
	  else
	    fputs((char*)f2->pos,fp);
	}
      else
	{
	  fputs("X",fp);
	}
    }
}

static void
lem_serialize_one(FILE *fp, struct ilem_form *f, struct serializer_frame *sfp)
{
  if (sfp->lit)
    fputs(sfp->lit,fp);
  else
    {
      if (*sfp->pre)
	fputs(sfp->pre, fp);
      lem_f2_serialize(fp, &f->f2);
      if (*sfp->pst)
	fputs(sfp->pst, fp);
    }
  
  if (f->ambig)
    {
      fputc('|', fp);
      lem_serialize_one(fp,f->ambig, sfp);
    }
}

static void
lem_serialize_one_xml(FILE *fp, struct ilem_form *f, struct serializer_frame *sfp)
{
  if (sfp->lit)
    {
      fputs((char*)xmlify((unsigned char*)sfp->lit),fp);
    }
  else
    {
      if (*sfp->pre)
	fputs((char*)xmlify((unsigned char *)sfp->pre), fp);

      /*warning("weird error: form->literal is NULL");*/
      if (f->f2.norm || f->f2.cf)
	{
	  if (strcmp((char*)f->f2.lang,"sux") && f->f2.norm)
	    fputs((char*)xmlify((unsigned char*)f->f2.norm),fp);
	  else
	    fputs((char*)xmlify(f->f2.cf),fp);
	  if (f->f2.sense)
	    fprintf(fp,"[%s]",(char*)xmlify((unsigned char *)f->f2.sense));
	  else
	    fprintf(fp,"[%s]",xmlify(f->f2.gw));
	  if (f->f2.epos && strcmp((char*)f->f2.pos,(char*)f->f2.epos))
	    fprintf(fp,"'%s",xmlify(f->f2.epos));
	  else
	    fputs((char*)xmlify(f->f2.pos),fp);
	}
      else if (f->f2.pos)
	{
	  fputs((char*)xmlify(f->f2.pos),fp);
	}
      else
	{
	  fputs("???",fp);
	}

      if (*sfp->pst)
	fputs((char*)xmlify((unsigned char *)sfp->pst), fp);
    }
  if (f->ambig)
    {
      fputc('|', fp);
      lem_serialize_one_xml(fp,f->ambig,sfp);
    }
}
void
lem_serialize(FILE *fp)
{
  int i;
  for (i = 0; i < lem_lines_used; ++i)
    {
      struct lem_save *lp;
      for (lp = &lem_lines[i]; lp; lp = lp->cont)
	fprintf(fp,"%s\n",lp->line);
      
      if (lem_lines[i].forms && *lem_lines[i].line != '#') /* suppress #lem: output for #note: @akk{pu} */
	{
	  int j;
	  fprintf(fp,"#lem: ");
	  for (j = 0; j < lem_lines[i].forms_used; ++j)
	    {
	      struct ilem_form *f = lem_lines[i].forms[j];
	      static struct serializer_frame sframe;

	      if (BIT_ISSET(f->instance_flags, ILEM_FLAG_SPARSE_SKIP))
		continue;

	      set_sframe(&sframe, f->literal);
	      if (sframe.lit)
		{
		  fputs(sframe.lit,fp);
		}
	      else
		{
#if 1 /* CHECK WHAT HAPPENS WITH n AND u */
		  if (!f->f2.pos || (*f->f2.pos == 'X' && !f->f2.pos[1]) || *f->f2.pos == 'L')
		    f->f2.pos = (unsigned char*)lem_base_type((char*)f->f2.form);
#endif
		  lem_serialize_one(fp,f,&sframe);
		  if (f->f2.parts)
		    {
#if 1
		      int k;
		      /* FIXME: parts should work with the sframe stuff */
		      for (k = 0; f->f2.parts[k]; ++k)
			{
			  fputc('&', fp);
			  lem_f2_serialize(fp, f->f2.parts[k]);
			}
#else
		      for (i = 0; f->f2.parts[i]; ++i)
			{
			  fputc('&', fp);
			  lem_f2_serialize(fp, f->f2.parts[i]);
			}
#endif
		    }
		}

	      if (j + 1 < lem_lines[i].forms_used)
		fputs("; ", fp);
	    }
	  fputc('\n',fp);
	  if (i < (lem_lines_used-1) && lem_lines[i+1].line && *lem_lines[i+1].line)
	    fputc('\n',fp);
	}
    }
}

void
lem_ods_serialize(FILE *fp)
{
  extern int ods_cols;
  /*extern char *odsods_fn;*/
  int i;
  fprintf(fp,"<lem cols=\"%d\">", ods_cols);
  for (i = 0; i < lem_lines_used; ++i)
    {
      if (lem_lines[i].forms)
	{
	  int j;
	  fprintf(fp,"<l n=\"%d\">",lem_lines[i].lnum);
	  for (j = 0; j < lem_lines[i].forms_used; ++j)
	    {
	      struct ilem_form *f = lem_lines[i].forms[j];
	      fprintf(fp,"<c n=\"%d\">",lem_lines[i].cells[j]);
	      if (use_literal(f->literal))
		{
		  fputs((char*)xmlify((unsigned char*)f->literal),fp);
		}
	      else
		{
		  static struct serializer_frame sframe;
		  set_sframe(&sframe, f->literal);

		  if (!f->f2.pos || (*f->f2.pos == 'X' && !f->f2.pos[1]) || *f->f2.pos == 'L')
		    f->f2.pos = (unsigned char*)lem_base_type((char*)f->f2.form);
		multi_entry:
		  lem_serialize_one_xml(fp,f,&sframe);
		  if (f->multi)
		    {
		      f = f->multi;
		      fputs("&amp;", fp);
		      goto multi_entry;
		    }
		}
#if 1
	      fputs("</c>",fp);
#else
	      if (j + 1 < lem_lines[i].forms_used)
		fputs("; ", fp);
#endif
	    }
	  fputs("</l>\n",fp);
	}
    }
  fputs("<xi:include xmlns:xi=\"http://www.w3.org/2001/XInclude\""
	" href=\"orig-content.xml\"/></lem>", 
	fp);
}

#if 0
void
lem_psu(char *psuline)
{
  char *psu_ngram, *psu_sig;
  
  if (!currlang->runsigs)
    {
      currlang->runsigs = mb_new(lemline_xcp->sigs->mb_sigsets);
      /* other runsigs initialization */
    }
  psu_ngram = psuline;
  while (*psu_ngram && !isspace(*psu_ngram))
    ++psu_ngram;
  while (isspace(*psu_ngram))
    ++psu_ngram;
  if (psu_sig)
    {
      char *psu_ngram_end = psu_sig;
      while (psu_ngram_end > psu_ngram && isspace(psu_ngram_end[-1]))
	--psu_ngram_end;
      *psu_ngram_end = '\0';
      psu_sig += 2;
      while (isspace(*psu_sig))
	++psu_sig;
      psu_register(file,lnum,currlang->runsigs,currlang,psu_ngram,NULL);
    }
}
#endif

#if 0
      if (fp->literal)
	{
	  if ('X' == *fp->literal && (!fp->literal[1] || isspace(fp->literal[1])))
	    fp->literal = NULL;
	}
      else if (fp->finds[0]->literal)
	{
	  if ('X' != *fp->literal && (!fp->literal[1] || !isspace(fp->literal[1])))
	    fp->literal = fp->finds[0]->literal;
	}
#endif

static void
set_sframe(struct serializer_frame *sfp, char *literal)
{
  char *X = NULL;
  if (!literal || ('X' == literal[0] && '\0' == literal[1]))
    {
      sfp->pre = sfp->pst = "";
      sfp->lit = NULL;
    }
  else
    {
      int found_X = 0;
      X = literal;
      while ((X = strchr(X, 'X')))
	{
	  if (X == literal && isspace(X[1]))
	    {
	      sfp->pre = "";
	      sfp->pst = X+1;
	      sfp->lit = NULL;
	      found_X = 1;
	      break;
	    }
	  else if (!X[1] && isspace(X[-1]))
	    {
	      *X = '\0';
	      sfp->pre = literal;
	      sfp->pst = "";
	      sfp->lit = NULL;
	      found_X = 1;
	      break;
	    }
	  else if (isspace(X[-1]) && isspace(X[1]))
	    {
	      sfp->pre = literal;
	      *X = '\0';
	      sfp->pst = X+1;
	      sfp->lit = NULL;
	      found_X = 1;
	      break;
	    }
	  else
	    {
	      ++X;
	    }
	}
      if (!found_X)
	{
	  sfp->pre = sfp->pst = "";
	  sfp->lit = literal;
	}      
    }
}
