#ifndef GLOBALLEXICALMODELUNLIMITED_H_
#define GLOBALLEXICALMODELUNLIMITED_H_

#include <stdexcept>
#include <string>
#include <vector>
#include "Factor.h"
#include "Phrase.h"
#include "TypeDef.h"
#include "Util.h"
#include "WordsRange.h"
#include "FeatureFunction.h"
#include "FactorTypeSet.h"
#include "Sentence.h"

#include "FFState.h"

#ifdef WITH_THREADS
#include <boost/thread/tss.hpp>
#endif

namespace Moses
{

class Factor;
class Phrase;
class Hypothesis;
class InputType;

/** Discriminatively trained global lexicon model
 * This is a implementation of Mauser et al., 2009's model that predicts
 * each output word from _all_ the input words. The intuition behind this
 * feature is that it uses context words for disambiguation
 */

class GlobalLexicalModelUnlimited : public StatelessFeatureFunction
{
	typedef std::map< char, short > CharHash;
	typedef std::map< std::string, short > StringHash;

  struct ThreadLocalStorage
  {
    const Sentence *input;
  };

private:
#ifdef WITH_THREADS
  boost::thread_specific_ptr<ThreadLocalStorage> m_local;
#else
  std::auto_ptr<ThreadLocalStorage> m_local;
#endif

  CharHash m_punctuationHash;

  std::vector< FactorType > m_inputFactors;
  std::vector< FactorType > m_outputFactors;
  bool m_unrestricted;

  bool m_sourceContext;
  bool m_biphrase;
  bool m_bitrigger;

  bool m_biasFeature;
  bool m_ignorePunctuation;

  std::set<std::string> m_vocabSource;
  std::set<std::string> m_vocabTarget;

public:
  GlobalLexicalModelUnlimited(const std::string &line);

  bool Load(const std::string &filePathSource, const std::string &filePathTarget);

  void InitializeForInput( Sentence const& in );

  const FFState* EmptyHypothesisState(const InputType &) const {
  	return new DummyState();
  }

  //TODO: This implements the old interface, but cannot be updated because
  //it appears to be stateful
  void Evaluate(const Hypothesis& cur_hypo,
  							ScoreComponentCollection* accumulator) const;

  void EvaluateChart(const ChartHypothesis& /* cur_hypo */,
  									 int /* featureID */,
  									 ScoreComponentCollection* ) const {
    throw std::logic_error("GlobalLexicalModelUnlimited not supported in chart decoder, yet");
  }

	void AddFeature(ScoreComponentCollection* accumulator,
			StringPiece sourceTrigger, StringPiece sourceWord, StringPiece targetTrigger,
			StringPiece targetWord) const;

};

}
#endif /* GLOBALLEXICALMODELUNLIMITED_H_ */
