/*  Copyright 2013 Daniel Wilson and Xavier Didelot.
 *
 *  main.h
 *  Part of ClonalFrameML
 *
 *  The myutils library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  The myutils library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *  
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the myutils library. If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef _MAIN_H_
#define _MAIN_H_
#include <iostream>
#include <newick.h>
#include <coalesce.h>
#include <coalescent_record.h>
#include <sstream>
#include <myutils.h>
#include <fstream>
#include <algorithm>
#include <DNA.h>
#include <mydouble.h>
#include <mutation.h>
#include <powell.h>
#include <argumentwizard.h>
#include <time.h>
#include <omp.h>

using std::cout;
using myutils::NewickTree;
using std::stringstream;
using myutils::error;
using myutils::ArgumentWizard;
using myutils::DATA_TYPE;

enum Nucleotide {Adenine=0, Guanine, Cytosine, Thymine, N_ambiguous};
enum ImportationState {Unimported=0, Imported};

marginal_tree convert_rooted_NewickTree_to_marginal_tree(NewickTree &newick, vector<string> &tip_labels, vector<string> &all_node_labels);
marginal_tree convert_unrooted_NewickTree_to_marginal_tree(NewickTree &newick, vector<string> &tip_labels, vector<string> &all_node_labels);
vector<int> compute_compatibility(DNA &fa, marginal_tree &tree, vector<bool> &anyN, bool purge_singletons=true);
NewickTree read_Newick(const char* newick_file);
Matrix<Nucleotide> FASTA_to_nucleotide(DNA &fa, vector<double> &empirical_nucleotide_frequencies, vector<bool> usesite);
void find_alignment_patterns(Matrix<Nucleotide> &nuc, vector<bool> &iscompat, vector<string> &pat, vector<int> &pat1, vector<int> &cpat, vector<int> &ipat);
vector< Matrix<double> > compute_HKY85_ptrans(const marginal_tree &ctree, const double kappa, const vector<double> &pi);
vector< Matrix<double> > compute_HKY85_ptrans(const marginal_tree &ctree, const double kappa, const vector<double> &pi, const double import_divergence, const bool excess_divergence_model);
Matrix<mydouble> compute_HKY85_ptrans(const double x, const double k, const vector<double> &pi);
Matrix<double> dcompute_HKY85_ptrans(const double x, const double kappa, const vector<double> &pi);
double HKY85_expected_rate(const vector<double> &n, const double kappa, const vector<double> &pi);
mydouble maximum_likelihood_ancestral_sequences(Matrix<Nucleotide> &nuc, marginal_tree &ctree, const double kappa, const vector<double> &pi, vector<int> &pat1, vector<int> &cpat, Matrix<Nucleotide> &node_sequence);
void write_newick(const marginal_tree &ctree, const vector<string> &all_node_names, const char* file_name);
void write_newick(const marginal_tree &ctree, const vector<string> &all_node_names, ofstream &fout);
void write_newick_node(const mt_node *node, const vector<string> &all_node_names, ofstream &fout);
void write_ancestral_fasta(Matrix<Nucleotide> &nuc, vector<string> &all_node_names, const char* file_name);
void write_ancestral_fasta(Matrix<Nucleotide> &nuc, vector<string> &all_node_names, ofstream &fout);
void write_position_cross_reference(vector<bool> &iscompat, vector<int> &ipat, const char* file_name);
void write_position_cross_reference(vector<bool> &iscompat, vector<int> &ipat, ofstream &fout);
mydouble maximum_likelihood_ClonalFrame_branch_allsites(const int dec_id, const int anc_id, const Matrix<Nucleotide> &node_nuc, const vector<bool> &iscompat, const vector<int> &ipat, const double kappa, const vector<double> &pi, const double branch_length, const double rho_over_theta, const double mean_import_length, const double import_divergence, vector<ImportationState> &is_imported);
mydouble maximum_likelihood_ClonalFrame_branch(const int dec_id, const int anc_id, const Matrix<Nucleotide> &node_nuc, const vector<bool> &iscompat, const vector<int> &ipat, const double kappa, const vector<double> &pi, const double branch_length, const double rho_over_theta, const double mean_import_length, const double import_divergence, vector<ImportationState> &is_imported);
double marginal_likelihood_ClonalFrame_branch(const int dec_id, const int anc_id, const Matrix<Nucleotide> &node_nuc, const vector<bool> &iscompat, const vector<int> &ipat, const double kappa, const vector<double> &pi, const double branch_length, const double rho_over_theta, const double mean_import_length, const double import_divergence);
mydouble likelihood_branch(const int dec_id, const int anc_id, const Matrix<Nucleotide> &node_nuc, const vector<int> &pat1, const vector<int> &cpat, const double kappa, const vector<double> &pinuc, const double branch_length);
bool string_to_bool(const string s, const string label="");
void write_importation_status(vector< vector<ImportationState> > &imported, vector<string> &all_node_names, vector<bool> &isBLC, vector<int> &compat, const char* file_name);
void write_importation_status(vector< vector<ImportationState> > &imported, vector<string> &all_node_names, vector<bool> &isBLC, vector<int> &compat, ofstream &fout);
void write_importation_status_intervals(vector< vector<ImportationState> > &imported, vector<string> &all_node_names, vector<bool> &isBLC, vector<int> &compat, const char* file_name);
void write_importation_status_intervals(vector< vector<ImportationState> > &imported, vector<string> &all_node_names, vector<bool> &isBLC, vector<int> &compat, ofstream &fout);

class orderNewickNodesByStatusAndAge : public std::binary_function<size_t,size_t,bool> {
public:
	const vector<NewickNode*> &root2tip;	// temporary ordering of Newick nodes from root to tips
	const vector<double> &ageroot2tip;		// corresponding age of each node in root2tip
	orderNewickNodesByStatusAndAge(const vector<NewickNode*> &root2tip_in, const vector<double> &ageroot2tip_in) : root2tip(root2tip_in), ageroot2tip(ageroot2tip_in) {
	}
	// Test if i is less than j
	bool operator()(size_t i, size_t j) const {
		// If i is a tip
		if(root2tip[i]->dec.size()==0) {
			// And j is not
			if(root2tip[j]->dec.size()!=0) return true;
			// If both are tips
			return ageroot2tip[i] < ageroot2tip[j];
		} else {
			// If j is
			if(root2tip[j]->dec.size()==0) return false;
			// Neither are tips
			return ageroot2tip[i] < ageroot2tip[j];
		}
	}
};

class orderNewickNodesByStatusLabelAndAge : public std::binary_function<size_t,size_t,bool> {
public:
	const vector<NewickNode*> &root2tip;	// temporary ordering of Newick nodes from root to tips
	const vector<double> &ageroot2tip;		// corresponding age of each node in root2tip
	const vector<size_t> &labelorder;		// The position where each node comes in the label order (for tips; the label is ignored for internal nodes)
	orderNewickNodesByStatusLabelAndAge(const vector<NewickNode*> &root2tip_in, const vector<double> &ageroot2tip_in, const vector<size_t> &labelorder_in) : 
	root2tip(root2tip_in), ageroot2tip(ageroot2tip_in), labelorder(labelorder_in) {
	}
	// Test if i is less than j
	bool operator()(size_t i, size_t j) const {
		if(root2tip[i]->dec.size()==0 && root2tip[j]->dec.size()!=0) {
			// If i is a tip and j is not
			return true;
		} else if(root2tip[i]->dec.size()==0 && root2tip[j]->dec.size()==0) {
			// If i and j are both tips
			// Then order by label
			if(labelorder[i]==labelorder[j]) {
				stringstream errTxt;
				errTxt << "orderNewickNodesByStatusLabelAndAge::operator(): ";
				errTxt << "tips cannot have the same label order";
				error(errTxt.str().c_str());
			}
			return labelorder[i] < labelorder[j];
		} else if(root2tip[i]->dec.size()!=0 && root2tip[j]->dec.size()==0) {
			// If i is not a tip but j is
			return false;
		} else {
			// If neither are tips
			// Then order by age
			return ageroot2tip[i] < ageroot2tip[j];
		}
	}
};

class ClonalFrameBranchFunction : public PowellFunction {
public:
	// References to non-member variables
	const mt_node &node;
	const Matrix<Nucleotide> &node_nuc;
	const vector<bool> &iscompat;
	const vector<int> &ipat;
	const double kappa;
	const vector<double> &pi;
	// True member variable
	vector<ImportationState> is_imported;
	mydouble ML;
	int neval;
	bool excess_divergence_model;
public:
	ClonalFrameBranchFunction(const mt_node &_node, const Matrix<Nucleotide> &_node_nuc, const vector<bool> &_iscompat, const vector<int> &_ipat, const double _kappa,
							  const vector<double> &_pi, bool _excess_divergence_model) : 
	node(_node), node_nuc(_node_nuc), iscompat(_iscompat), ipat(_ipat), kappa(_kappa), pi(_pi), neval(0), excess_divergence_model(_excess_divergence_model) {};
	double f(const vector<double>& x) {
		++neval;
		if(x.size()!=4) error("ClonalFrameBranchFunction::f(): 4 arguments required");
		const int dec_id = node.id;
		const int anc_id = node.ancestor->id;
		const double branch_length = pow(10.,x[0]);
		const double rho_over_theta = pow(10.,x[1]);
		const double mean_import_length = pow(10.,x[2]);
		const double import_divergence = (excess_divergence_model) ? branch_length + pow(10.,x[3]) : pow(10.,x[3]);
		ML = maximum_likelihood_ClonalFrame_branch(dec_id,anc_id,node_nuc,iscompat,ipat,kappa,pi,branch_length,rho_over_theta,mean_import_length,import_divergence,is_imported);
		return -ML.LOG();
	}
};

class ClonalFrameFunction : public PowellFunction {
public:
	// References to non-member variables
	const marginal_tree &ctree;
	const Matrix<Nucleotide> &node_nuc;
	const vector<bool> &iscompat;
	const vector<int> &ipat;
	const double kappa;
	const vector<double> &pi;
	// True member variable
	vector< vector<ImportationState> > is_imported;
	vector< mydouble > ML;
	int neval;
	bool excess_divergence_model;
	clock_t last_update;
	const bool multithread;
public:
	ClonalFrameFunction(const marginal_tree &_ctree, const Matrix<Nucleotide> &_node_nuc, const vector<bool> &_iscompat, const vector<int> &_ipat, const double _kappa,
							  const vector<double> &_pi, bool _excess_divergence_model, const bool _multithread) : 
	ctree(_ctree), node_nuc(_node_nuc), iscompat(_iscompat), ipat(_ipat), kappa(_kappa), pi(_pi), is_imported(_ctree.size-2), ML(_ctree.size-2), neval(0), 
	excess_divergence_model(_excess_divergence_model), last_update(clock()), multithread(_multithread) {};
	double f(const vector<double>& x) {
		++neval;
		if((clock()-last_update)/CLOCKS_PER_SEC>60.0) {
			cout << "Done " << neval << " iterations" << endl;
			last_update = clock();
		}
		// Process parameters
		const int nparam = ctree.size-2+3;
		if(x.size()!=nparam) {
			stringstream errTxt;
			errTxt << "ClonalFrameFunction::f(): " << nparam << " arguments required";
			error(errTxt.str().c_str());
		}
		const double rho_over_theta = pow(10.,x[0]);
		const double mean_import_length = pow(10.,x[1]);
		const double import_divergence_base = pow(10.,x[2]);
		// Calculate likelihood
		int i;
		for(i=0;i<ctree.size-2;i++) {
			const double branch_length = pow(10.,x[3+i]);
			const double import_divergence = (excess_divergence_model) ? import_divergence_base + branch_length : import_divergence_base;
			const mt_node &node = ctree.node[i];
			const int dec_id = node.id;
			const int anc_id = node.ancestor->id;
			ML[i] = maximum_likelihood_ClonalFrame_branch(dec_id,anc_id,node_nuc,iscompat,ipat,kappa,pi,branch_length,rho_over_theta,mean_import_length,import_divergence,is_imported[i]);
		}
		// Separate: not included in the parallelizable code
		mydouble treeML = 1.0;
		for(i=0;i<ctree.size-2;i++) {
			treeML *= ML[i];
		}
		// The penultimate node (i==ctree.size-2) is a special case: do not try to optimize its branch length nor calculate its likelihood (will be 1)
		// Return minus log-likelihood
		return -treeML.LOG();
	}
};

class ClonalFrameApproxBranchLengthFunction : public PowellFunction {
public:
	// References to non-member variables
	const marginal_tree &ctree;
	const Matrix<Nucleotide> &node_nuc;
	const vector<bool> &iscompat;
	const vector<int> &ipat;
	const double kappa;
	const vector<double> &pi;
	// True member variable
	vector< vector<ImportationState> > is_imported;
	vector< mydouble > ML;
	int neval;
	bool excess_divergence_model;
	clock_t last_update;
	vector<double> adjusted_pmut;
	const double min_branch_length;
	vector<double> branch_length_hat;
public:
	ClonalFrameApproxBranchLengthFunction(const marginal_tree &_ctree, const Matrix<Nucleotide> &_node_nuc, const vector<bool> &_iscompat, const vector<int> &_ipat, const double _kappa,
						const vector<double> &_pi, bool _excess_divergence_model, const double _min_branch_length=0.0000001) : 
	ctree(_ctree), node_nuc(_node_nuc), iscompat(_iscompat), ipat(_ipat), kappa(_kappa), pi(_pi), is_imported(_ctree.size-2), ML(_ctree.size-2), neval(0), 
	excess_divergence_model(_excess_divergence_model), last_update(clock()), adjusted_pmut(_ctree.size-2), min_branch_length(_min_branch_length), branch_length_hat(_ctree.size-2) {
		int i,j;
		for(i=0;i<ctree.size-2;i++) {
			const int dec = i;
			const int anc = ctree.node[i].ancestor->id;
			vector<double> n(4,0.0);
			double nmut = 0;
			for(j=0;j<ipat.size();j++) {
				Nucleotide from = node_nuc[anc][ipat[j]];
				Nucleotide to = node_nuc[dec][ipat[j]];
				n[(int)from]++;
				if(from!=to) ++nmut;
			}
			// Branch-specific observed proportion of mutant sites adjusted for base composition of parent branch. Generally, the denominator should be close to the number of sites.
			adjusted_pmut[i] = nmut/HKY85_expected_rate(n,kappa,pi);
			if(excess_divergence_model) {
				cout << "WARNING: excess divergence model not available for ClonalFrameApproxBranchLengthFunction, ignoring.";
			}
		}
	};
	double f(const vector<double>& x) {
		++neval;
		if((clock()-last_update)/CLOCKS_PER_SEC>60.0) {
			cout << "Done " << neval << " iterations" << endl;
			last_update = clock();
		}
		// Process parameters
		const int nparam = 3;
		if(x.size()!=nparam) {
			stringstream errTxt;
			errTxt << "ClonalFrameApproxBranchLengthFunction::f(): " << nparam << " arguments required";
			error(errTxt.str().c_str());
		}
		const double rho_over_theta = pow(10.,x[0]);
		const double mean_import_length = pow(10.,x[1]);
		const double import_divergence_base = pow(10.,x[2]);
		// Calculate likelihood
		int i;
		for(i=0;i<ctree.size-2;i++) {
			// Fast approximate estimate of branch length
			branch_length_hat[i] = (1/mean_import_length*adjusted_pmut[i])/(1/mean_import_length+rho_over_theta*(import_divergence_base-adjusted_pmut[i]));
			if(branch_length_hat[i]<min_branch_length) branch_length_hat[i]=min_branch_length;
			const mt_node &node = ctree.node[i];
			const int dec_id = node.id;
			const int anc_id = node.ancestor->id;
			ML[i] = maximum_likelihood_ClonalFrame_branch(dec_id,anc_id,node_nuc,iscompat,ipat,kappa,pi,branch_length_hat[i],rho_over_theta,mean_import_length,import_divergence_base,is_imported[i]);
		}
		// Separate: not included in the parallelizable code
		mydouble treeML = 1.0;
		for(i=0;i<ctree.size-2;i++) {
			treeML *= ML[i];
		}
		// The penultimate node (i==ctree.size-2) is a special case: do not try to optimize its branch length nor calculate its likelihood (will be 1)
		// Return minus log-likelihood
		return -treeML.LOG();
	}
};


class ClonalFrameParameterFunction : public PowellFunction {
public:
	// References to non-member variables
	const marginal_tree &ctree;
	const Matrix<Nucleotide> &node_nuc;
	const vector<bool> &iscompat;
	const vector<int> &ipat;
	const double kappa;
	const vector<double> &pi;
	// True member variable
	vector< vector<ImportationState> > is_imported;
	vector< mydouble > ML;
	int neval;
	bool excess_divergence_model;
	clock_t last_update;
	const bool multithread;
public:
	ClonalFrameParameterFunction(const marginal_tree &_ctree, const Matrix<Nucleotide> &_node_nuc, const vector<bool> &_iscompat, const vector<int> &_ipat, const double _kappa,
								 const vector<double> &_pi, bool _excess_divergence_model, const bool _multithread) : 
	ctree(_ctree), node_nuc(_node_nuc), iscompat(_iscompat), ipat(_ipat), kappa(_kappa), pi(_pi), is_imported(_ctree.size-2), ML(_ctree.size-2), neval(0), 
	excess_divergence_model(_excess_divergence_model), last_update(clock()), multithread(_multithread) {};
	double f(const vector<double>& x) {
		++neval;
		if((clock()-last_update)/CLOCKS_PER_SEC>60.0) {
			cout << "Done " << neval << " iterations" << endl;
			last_update = clock();
		}
		// Process parameters
		const int nparam = 3;
		if(x.size()!=nparam) {
			stringstream errTxt;
			errTxt << "ClonalFrameParameterFunction::f(): " << nparam << " arguments required";
			error(errTxt.str().c_str());
		}
		const double rho_over_theta = pow(10.,x[0]);
		const double mean_import_length = pow(10.,x[1]);
		const double import_divergence_base = pow(10.,x[2]);
		// Calculate likelihood
		int i;
		for(i=0;i<ctree.size-2;i++) {
			const mt_node &node = ctree.node[i];
			const double branch_length = node.edge_time;
			const double import_divergence = (excess_divergence_model) ? import_divergence_base + branch_length : import_divergence_base;
			const int dec_id = node.id;
			const int anc_id = node.ancestor->id;
			ML[i] = maximum_likelihood_ClonalFrame_branch(dec_id,anc_id,node_nuc,iscompat,ipat,kappa,pi,branch_length,rho_over_theta,mean_import_length,import_divergence,is_imported[i]);
		}
		// Separate: not included in the parallelizable code
		mydouble treeML = 1.0;
		for(i=0;i<ctree.size-2;i++) {
			treeML *= ML[i];
		}
		// The penultimate node (i==ctree.size-2) is a special case: do not try to optimize its branch length nor calculate its likelihood (will be 1)
		// Return minus log-likelihood
		return -treeML.LOG();
	}
};

class ClonalFrameBranchLengthFunction : public PowellFunction {
public:
	// References to non-member variables
	const mt_node &node;
	const Matrix<Nucleotide> &node_nuc;
	const vector<bool> &iscompat;
	const vector<int> &ipat;
	const double kappa;
	const vector<double> &pi;
	// True member variable
	vector<ImportationState> is_imported;
	mydouble ML;
	int neval;
	bool excess_divergence_model;
	double rho_over_theta, mean_import_length, import_divergence;
	const bool multithread;
public:
	ClonalFrameBranchLengthFunction(const mt_node &_node, const Matrix<Nucleotide> &_node_nuc, const vector<bool> &_iscompat, const vector<int> &_ipat, const double _kappa,
							  const vector<double> &_pi, bool _excess_divergence_model, const double _rho_over_theta, const double _mean_import_length, const double _import_divergence, const bool _multithread) : 
	node(_node), node_nuc(_node_nuc), iscompat(_iscompat), ipat(_ipat), kappa(_kappa), pi(_pi), neval(0), excess_divergence_model(_excess_divergence_model),
	rho_over_theta(_rho_over_theta), mean_import_length(_mean_import_length), import_divergence(_import_divergence), multithread(_multithread) {};
	double f(const vector<double>& x) {
		++neval;
		if(x.size()!=1) error("ClonalFrameBranchLengthFunction::f(): 1 argument required");
		const int dec_id = node.id;
		const int anc_id = node.ancestor->id;
		const double branch_length = pow(10.,x[0]);
		const double final_import_divergence = (excess_divergence_model) ? branch_length + import_divergence : import_divergence;
		ML = maximum_likelihood_ClonalFrame_branch(dec_id,anc_id,node_nuc,iscompat,ipat,kappa,pi,branch_length,rho_over_theta,mean_import_length,final_import_divergence,is_imported);
		return -ML.LOG();
	}
};

class ClonalFrameRhoPerBranchFunction : public PowellFunction {
public:
	// References to non-member variables
	const mt_node &node;
	const Matrix<Nucleotide> &node_nuc;
	const vector<bool> &iscompat;
	const vector<int> &ipat;
	const double kappa;
	const vector<double> &pi;
	vector<ImportationState> &is_imported;
	// True member variable
	double ML;
	int neval;
	bool excess_divergence_model;
	const bool multithread;
	double crude_branch_length;
	double min_branch_length;
public:
	ClonalFrameRhoPerBranchFunction(const mt_node &_node, const Matrix<Nucleotide> &_node_nuc, const vector<bool> &_iscompat, const vector<int> &_ipat, const double _kappa,
									const vector<double> &_pi, bool _excess_divergence_model, const bool _multithread, vector<ImportationState> &_is_imported, const double _crude_branch_length, const double _min_branch_length) : 
	node(_node), node_nuc(_node_nuc), iscompat(_iscompat), ipat(_ipat), kappa(_kappa), pi(_pi), neval(0),
	excess_divergence_model(_excess_divergence_model), multithread(_multithread), is_imported(_is_imported), crude_branch_length(_crude_branch_length), min_branch_length(_min_branch_length) {
		if(!excess_divergence_model) error("ClonalFrameRhoPerBranchFunction::f(): excess_divergence_model is mandatory");	
	};
	double f(const vector<double>& x) {
		++neval;
		// Process parameters
		if(!(x.size()==3 || x.size()==4)) error("ClonalFrameRhoPerBranchFunction::f(): 3 or 4 arguments required");
		const double rho_over_theta = pow(10.,x[0]);
		const double import_ratio = 1.0/(1.0+pow(10.,-x[1]));
		const double import_divergence = pow(10.,x[2]);
		double branch_length;
		if(x.size()==3) {
			// Constrain so that the expected number of substitutions equals crude_branch_length
			// crude_branch_length = branch_length + import_ratio/(1+import_ratio)*branch_length*(2+import_divergence)
			//                     = branch_length*(1 + import_ratio/(1+import_ratio)*(2+import_divergence))
			// so    branch_length = crude_branch_length/(1 + import_ratio/(1+import_ratio)*(2+import_divergence))
			branch_length = crude_branch_length/(1.0+import_ratio/(1.0+import_ratio)*(2.0+import_divergence));
		} else {
			branch_length = pow(10.,x[3]);
		}
		if(branch_length<min_branch_length) branch_length = min_branch_length;
		const int dec_id = node.id;
		const int anc_id = node.ancestor->id;
		const double mean_import_length = import_ratio/branch_length/rho_over_theta;
		const double final_import_divergence = (excess_divergence_model) ? branch_length*(2.0 + import_divergence) : import_divergence;
		// Calculate likelihood
		//ML = maximum_likelihood_ClonalFrame_branch(dec_id,anc_id,node_nuc,iscompat,ipat,kappa,pi,branch_length,rho_over_theta,mean_import_length,final_import_divergence,is_imported);
		//return -ML.LOG();
		ML = marginal_likelihood_ClonalFrame_branch(dec_id,anc_id,node_nuc,iscompat,ipat,kappa,pi,branch_length,rho_over_theta,mean_import_length,final_import_divergence);
		return -ML;
	}
};

class ClonalFrameRescaleBranchFunction : public PowellFunction {
public:
	// References to non-member variables
	const mt_node &node;
	const Matrix<Nucleotide> &node_nuc;
	const vector<int> &pat1;
	const vector<int> &cpat;
	const double kappa;
	const vector<double> &pi;
	// True member variable
	mydouble ML;
	int neval;
	const bool multithread;
	double crude_branch_length;
	double min_branch_length;
public:
	ClonalFrameRescaleBranchFunction(const mt_node &_node, const Matrix<Nucleotide> &_node_nuc, const vector<int> &_pat1, const vector<int> &_cpat, const double _kappa,
									const vector<double> &_pi, const bool _multithread, const double _crude_branch_length, const double _min_branch_length) : 
	node(_node), node_nuc(_node_nuc), pat1(_pat1), cpat(_cpat), kappa(_kappa), pi(_pi), neval(0),
	multithread(_multithread), crude_branch_length(_crude_branch_length), min_branch_length(_min_branch_length) {};
	double f(const vector<double>& x) {
		++neval;
		// Process parameters
		if(!(x.size()==1)) error("ClonalFrameRescaleBranchFunction::f(): 1 argument required");
		double branch_length = pow(10.,x[0]);
		if(branch_length<min_branch_length) branch_length = min_branch_length;
		const int dec_id = node.id;
		const int anc_id = node.ancestor->id;
		// Calculate likelihood
		ML = likelihood_branch(dec_id,anc_id,node_nuc,pat1,cpat,kappa,pi,branch_length);
		return -ML.LOG();
	}
};

class ClonalFrameSingleRho : public PowellFunction {
public:
	// References to non-member variables
	const marginal_tree &tree;
	const Matrix<Nucleotide> &node_nuc;
	const vector<bool> &iscompat;
	const vector<int> &ipat;
	const double kappa;
	const vector<double> &pi;
	vector< vector<ImportationState> > &is_imported;
	// True member variable
	const bool use_viterbi;
	double ML;
	int neval;
	bool excess_divergence_model;
	const bool multithread;
	const vector<double> &substitutions_per_branch;
	double min_branch_length;
public:
	ClonalFrameSingleRho(const bool _use_viterbi, const marginal_tree &_tree, const Matrix<Nucleotide> &_node_nuc, const vector<bool> &_iscompat, const vector<int> &_ipat, const double _kappa,
									const vector<double> &_pi, bool _excess_divergence_model, const bool _multithread, vector< vector<ImportationState> > &_is_imported, const vector<double> _substitutions_per_branch, const double _min_branch_length) : 
	use_viterbi(_use_viterbi), tree(_tree), node_nuc(_node_nuc), iscompat(_iscompat), ipat(_ipat), kappa(_kappa), pi(_pi), neval(0),
	excess_divergence_model(_excess_divergence_model), multithread(_multithread), is_imported(_is_imported), substitutions_per_branch(_substitutions_per_branch), min_branch_length(_min_branch_length) {
		if(excess_divergence_model) error("ClonalFrameSingleRho::f(): excess_divergence_model not implemented");	
	};
	double f(const vector<double>& x) {
		++neval;
		// Process parameters
		if(!(x.size()==3)) error("ClonalFrameSingleRho::f(): 3 arguments required");
		const double rho_over_theta = get_rho_over_theta(x);
		const double mean_import_length = get_mean_import_length(x);
		const double import_divergence = get_import_divergence(x);
		ML = 0.0;
		int i;
		for(i=0;i<tree.size-2;i++) {
			const mt_node &node = tree.node[i];
			const double branch_length = get_branch_length(substitutions_per_branch[i],x);
			const int dec_id = node.id;
			const int anc_id = node.ancestor->id;
			if(use_viterbi) {
				// Calculate likelihood: Viterbi
				ML += maximum_likelihood_ClonalFrame_branch(dec_id,anc_id,node_nuc,iscompat,ipat,kappa,pi,branch_length,rho_over_theta,mean_import_length,import_divergence,is_imported[i]).LOG();
			} else {
				// Calculate likelihood: Forward algorithm
				ML += marginal_likelihood_ClonalFrame_branch(dec_id,anc_id,node_nuc,iscompat,ipat,kappa,pi,branch_length,rho_over_theta,mean_import_length,import_divergence);
			}
		}
		return -ML;
	}
	double get_rho_over_theta(const vector<double>& x) {
		return pow(10.,x[0]);
	}
	double get_mean_import_length(const vector<double>& x) {
		return pow(10.,x[1]);
	}
	double get_import_divergence(const vector<double>& x) {
		return pow(10.,x[2]);
	}
	double get_branch_length(const double expected_number_of_substitutions, const vector<double>& x) {
		if(excess_divergence_model) error("ClonalFrameSingleRho::f(): excess_divergence_model not implemented");
		// Constrain EACH BRANCH so that the expected number of substitutions matches the ancestral state reconstruction
		// SUBJECT to a minimum (positive) branch length
		// Standard model of divergence: equal for every branch
		// Pr(import) = (rho*tau*T/2)/(1+rho*tau*T/2) = (branch_length*rho/theta*tau)/(1+branch_length*rho/theta*tau)
		// where tau = mean import length
		// E(# substitutions) = pr(no import) * E(# mutations) + pr(import) * import_divergence
		//                    = branch_length * (1 + rho/theta*tau*import_divergence)/(1+branch_length*rho/theta*tau)
		// so   branch_length = E(# substitutions)/(1 + rho/theta*tau*(import_divergence-E(# substitutions)))
		const double rho_over_theta = get_rho_over_theta(x);
		const double mean_import_length = get_mean_import_length(x);
		const double import_divergence = get_import_divergence(x);
		double b = expected_number_of_substitutions/(1.0+rho_over_theta*mean_import_length*(import_divergence-expected_number_of_substitutions));
		if(expected_number_of_substitutions>=import_divergence) b = expected_number_of_substitutions;
		if(b!=b || b<min_branch_length) b = min_branch_length;
		return b;
	}
};

#endif // _MAIN_H_