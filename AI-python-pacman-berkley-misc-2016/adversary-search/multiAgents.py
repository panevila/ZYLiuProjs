# multiAgents.py
# --------------
# Licensing Information: Please do not distribute or publish solutions to this
# project. You are free to use and extend these projects for educational
# purposes. The Pacman AI projects were developed at UC Berkeley, primarily by
# John DeNero (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# For more info, see http://inst.eecs.berkeley.edu/~cs188/sp09/pacman.html

from util import manhattanDistance
from game import Directions
import random, util

from game import Agent

class ReflexAgent(Agent):
  """
    A reflex agent chooses an action at each choice point by examining
    its alternatives via a state evaluation function.

    The code below is provided as a guide.  You are welcome to change
    it in any way you see fit, so long as you don't touch our method
    headers.
  """


  def getAction(self, gameState):
    """
    You do not need to change this method, but you're welcome to.

    getAction chooses among the best options according to the evaluation function.

    Just like in the previous project, getAction takes a GameState and returns
    some Directions.X for some X in the set {North, South, West, East, Stop}
    """
    # Collect legal moves and successor states
    legalMoves = gameState.getLegalActions()

    # Choose one of the best actions
    scores = [self.evaluationFunction(gameState, action) for action in legalMoves]
    bestScore = max(scores)
    bestIndices = [index for index in range(len(scores)) if scores[index] == bestScore]
    chosenIndex = random.choice(bestIndices) # Pick randomly among the best

    "Add more of your code here if you want to"

    return legalMoves[chosenIndex]

  def evaluationFunction(self, currentGameState, action):
    """
    Design a better evaluation function here.

    The evaluation function takes in the current and proposed successor
    GameStates (pacman.py) and returns a number, where higher numbers are better.

    The code below extracts some useful information from the state, like the
    remaining food (newFood) and Pacman position after moving (newPos).
    newScaredTimes holds the number of moves that each ghost will remain
    scared because of Pacman having eaten a power pellet.

    Print out these variables to see what you're getting, then combine them
    to create a masterful evaluation function.
    """
    # Useful information you can extract from a GameState (pacman.py)
    successorGameState = currentGameState.generatePacmanSuccessor(action)
    newPos = successorGameState.getPacmanPosition()
    newFood = successorGameState.getFood()
    newGhostStates = successorGameState.getGhostStates()
    newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]

    "*** YOUR CODE HERE ***"
    foodPosList = newFood.asList()
    nFoods = len(foodPosList)
    minFoodDist = float("inf")
    maxFoodDist = 0
    if len(newFood.asList())==0: return float("inf")

    for foodPos in foodPosList:
        thisDist = util.manhattanDistance(newPos, foodPos)
        if thisDist<minFoodDist:
            minFoodDist = thisDist
        if thisDist>maxFoodDist:
            maxFoodDist = thisDist
    
    ghostDist = util.manhattanDistance(newPos, successorGameState.getGhostPosition(1))
    if(len(newGhostStates)==2):
        ghostDist2 = util.manhattanDistance(newPos, successorGameState.getGhostPosition(2))
        if ghostDist2<ghostDist:
            ghostDist = ghostDist2
        
    if ghostDist<=1: return float("-inf")

    result = nFoods*(-10000) \
           + minFoodDist*(-400) \
           + maxFoodDist*(-100) \
           + sum(newScaredTimes)*80 \
           + ghostDist*200
    return result
    # return successorGameState.getScore()

def scoreEvaluationFunction(currentGameState):
  """
    This default evaluation function just returns the score of the state.
    The score is the same one displayed in the Pacman GUI.

    This evaluation function is meant for use with adversarial search agents
    (not reflex agents).
  """
  return currentGameState.getScore()

class MultiAgentSearchAgent(Agent):
  """
    This class provides some common elements to all of your
    multi-agent searchers.  Any methods defined here will be available
    to the MinimaxPacmanAgent, AlphaBetaPacmanAgent & ExpectimaxPacmanAgent.

    You *do not* need to make any changes here, but you can if you want to
    add functionality to all your adversarial search agents.  Please do not
    remove anything, however.

    Note: this is an abstract class: one that should not be instantiated.  It's
    only partially specified, and designed to be extended.  Agent (game.py)
    is another abstract class.
  """

  def __init__(self, evalFn = 'scoreEvaluationFunction', depth = '2'):
    self.index = 0 # Pacman is always agent index 0
    self.evaluationFunction = util.lookup(evalFn, globals())
    self.depth = int(depth)

class MinimaxAgent(MultiAgentSearchAgent):
  """
    Your minimax agent (question 2)
  """
  def getNextAgent(self, state, agentIndex):
    nAgent = state.getNumAgents()
    if agentIndex == nAgent-1:
      return 0
    return agentIndex+1

  def getMinimaxAction(self, state, depth, agentIndex, agentFlags):
    #initialization
    newDepth = depth
    minimaxAction = Directions.STOP
    evalVal = 0

    if agentFlags[agentIndex]==0:
      minimaxVal = float("-inf")
    else:
      minimaxVal = float("inf")

    # check agent turn status
    if agentIndex==state.getNumAgents()-1:
      #if this is the last agent in list, we gain 1 depth in next recursion call
      newDepth = depth + 1
    #getSucessor
    actions = state.getLegalActions(agentIndex)
    for action in actions:
      succState = state.generateSuccessor(agentIndex, action)
      if newDepth>self.depth or succState.isWin() or succState.isLose():
        #if it is end of depth
        evalVal = self.evaluationFunction(succState)
      else:
        #if we need to see deeper (minimaxAction returned here is not used)
        evalVal, tmpAction = self.getMinimaxAction(succState, newDepth, self.getNextAgent(state, agentIndex), agentFlags)
      
      if (agentFlags[agentIndex]==0 and evalVal>minimaxVal) or (agentFlags[agentIndex]==1 and evalVal<minimaxVal):
        minimaxVal = evalVal
        minimaxAction = action
    
    return minimaxVal, minimaxAction

  def getAction(self, gameState):
    """
      Returns the minimax action from the current gameState using self.depth
      and self.evaluationFunction.

      Here are some method calls that might be useful when implementing minimax.

      gameState.getLegalActions(agentIndex):
        Returns a list of legal actions for an agent
        agentIndex=0 means Pacman, ghosts are >= 1

      Directions.STOP:
        The stop direction, which is always legal

      gameState.generateSuccessor(agentIndex, action):
        Returns the successor game state after an agent takes an action

      gameState.getNumAgents():
        Returns the total number of agents in the game
    """
    "*** YOUR CODE HERE ***"
    agentFlags = [0,1,1,1,1,1,1,1,1,1] # 1 pacman, other are ghosts

    actionTuple = self.getMinimaxAction(gameState, 1, 0, agentFlags)
    return actionTuple[1]
    # return Directions.STOP

    

class AlphaBetaAgent(MultiAgentSearchAgent):
  """
    Your minimax agent with alpha-beta pruning (question 3)
  """
  def getNextAgent(self, state, agentIndex):
    nAgent = state.getNumAgents()
    if agentIndex == nAgent-1:
      return 0
    return agentIndex+1

  def getMinimaxAction(self, state, depth, agentIndex, agentFlags, a=float("-inf"), b=float("inf")):
    #initialization
    newDepth = depth
    minimaxAction = Directions.STOP
    evalVal = 0

    if agentFlags[agentIndex]==0:
      minimaxVal = float("-inf")
    else:
      minimaxVal = float("inf")

    # check agent turn status
    if agentIndex==state.getNumAgents()-1:
      #if this is the last agent in list, we gain 1 depth in next recursion call
      newDepth = depth + 1
    #getSucessor
    actions = state.getLegalActions(agentIndex)
    for action in actions:
      succState = state.generateSuccessor(agentIndex, action)
      if newDepth>self.depth or succState.isWin() or succState.isLose():
        #if it is end of depth
        evalVal = self.evaluationFunction(succState)
      else:
        #if we need to see deeper (minimaxAction returned here is not used)
        evalVal, tmpAction = self.getMinimaxAction(succState, newDepth, self.getNextAgent(state, agentIndex), agentFlags, a, b)
        
      if agentFlags[agentIndex]==0 and evalVal>minimaxVal:
        minimaxVal = evalVal
        minimaxAction = action
        #if the next node is a min node, we can set a
        if agentFlags[self.getNextAgent(state, agentIndex)] == 1:
          a = evalVal
      
      elif agentFlags[agentIndex]==1 and evalVal<minimaxVal:
        minimaxVal = evalVal
        minimaxAction = action
        #if the next node is a max node, we can set b
        if agentFlags[self.getNextAgent(state, agentIndex)] == 0:
          b = evalVal
      
      if (agentFlags[agentIndex]==1 and evalVal < a) or (agentFlags[agentIndex]==0 and evalVal > b):
        break

    return minimaxVal, minimaxAction
  
  def getAction(self, gameState):
    """
      Returns the minimax action using self.depth and self.evaluationFunction
    """
    "*** YOUR CODE HERE ***"
    agentFlags = [0,1,1,1,1,1,1,1,1,1] # 1 pacman, other are ghosts
    actionTuple = self.getMinimaxAction(gameState, 1, 0, agentFlags)
    return actionTuple[1]

class ExpectimaxAgent(MultiAgentSearchAgent):
  """
    Your expectimax agent (question 4)
  """
  def getNextAgent(self, state, agentIndex):
    nAgent = state.getNumAgents()
    if agentIndex == nAgent-1:
      return 0
    return agentIndex+1

  def getExpectimaxVal(self, state, depth, agentIndex, agentFlags):
    #initialization
    newDepth = depth
    expectimaxAction = Directions.STOP
    returnVal = 0
    tmpVal = 0
    expectimaxVal = float("-inf")
    # check agent turn status
    if agentIndex==state.getNumAgents()-1:
      #if this is the last agent in list, we gain 1 depth in next recursion call
      newDepth = depth + 1
    #getSucessor
    actions = state.getLegalActions(agentIndex)

    #if this function is called for a max Node, return max value
    if agentFlags[agentIndex]==0:
      for action in actions:
        if action == Directions.STOP:
          continue
        succState = state.generateSuccessor(agentIndex, action)
        if newDepth>self.depth or succState.isWin() or succState.isLose():
          #if it is end of depth
          tmpVal = self.evaluationFunction(succState)
        else:
          #if we need to see deeper (minimaxAction returned here is not used)
          tmpVal = self.getExpectimaxVal(succState, newDepth, self.getNextAgent(state, agentIndex), agentFlags)[0]
        
        if tmpVal > expectimaxVal:
          expectimaxVal = tmpVal
          expectimaxAction = action
      returnVal = expectimaxVal

    #if this function is called for a expect Node, calculate average
    else:
      tmpN = 0
      for action in actions:
        if action == Directions.STOP:
          continue
        succState = state.generateSuccessor(agentIndex, action)
        if newDepth>self.depth or succState.isWin() or succState.isLose():
          #if it is end of depth
          tmpVal = tmpVal + self.evaluationFunction(succState)
        else:
          #if we need to see deeper (minimaxAction returned here is not used)
          tmpVal = tmpVal + self.getExpectimaxVal(succState, newDepth, self.getNextAgent(state, agentIndex), agentFlags)[0]
        tmpN = tmpN+1
      returnVal = tmpVal/tmpN
    
    return returnVal, expectimaxAction

  def getAction(self, gameState):
    """
      Returns the expectimax action using self.depth and self.evaluationFunction

      All ghosts should be modeled as choosing uniformly at random from their
      legal moves.
    """
    "*** YOUR CODE HERE ***"
    agentFlags = [0,1,1,1,1,1,1,1,1,1] # 1 pacman, other are ghosts
    actionTuple = self.getExpectimaxVal(gameState, 1, 0, agentFlags)
    return actionTuple[1]

def betterEvaluationFunction(currentGameState):
  """
    Your extreme ghost-hunting, pellet-nabbing, food-gobbling, unstoppable
    evaluation function (question 5).

    DESCRIPTION: <write something here so we know what you did>
  """
  "*** YOUR CODE HERE ***"
  # Useful information you can extract from a GameState (pacman.py)
  newPos = currentGameState.getPacmanPosition()
  newFood = currentGameState.getFood()
  newGhostStates = currentGameState.getGhostStates()
  newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]

  foodPosList = newFood.asList()
  nFoods = len(foodPosList)
  minFoodDist = float("inf")
  maxFoodDist = 0
  if len(newFood.asList())==0: return float("inf")

  for foodPos in foodPosList:
      thisDist = util.manhattanDistance(newPos, foodPos)
      if thisDist<minFoodDist:
          minFoodDist = thisDist
      if thisDist>maxFoodDist:
          maxFoodDist = thisDist
    
  ghostDist = util.manhattanDistance(newPos, currentGameState.getGhostPosition(1))
  if(len(newGhostStates)==2):
      ghostDist2 = util.manhattanDistance(newPos, currentGameState.getGhostPosition(2))
      if ghostDist2<ghostDist:
          ghostDist = ghostDist2
  if ghostDist<=1: return float("-inf")

  result = nFoods*(-10000) \
        + minFoodDist*(-600) \
        + maxFoodDist*(-150) \
        + sum(newScaredTimes)*800000 \
        + ghostDist*100
  # print (nFoods*(-10000),minFoodDist*(-800),maxFoodDist*(-200),sum(newScaredTimes)*6000000/ghostDist,ghostDist*200)
  # print result
  return result

# Abbreviation
better = betterEvaluationFunction

class ContestAgent(MultiAgentSearchAgent):
  """
    Your agent for the mini-contest
  """

  def getAction(self, gameState):
    """
      Returns an action.  You can use any method you want and search to any depth you want.
      Just remember that the mini-contest is timed, so you have to trade off speed and computation.

      Ghosts don't behave randomly anymore, but they aren't perfect either -- they'll usually
      just make a beeline straight towards Pacman (or away from him if they're scared!)
    """
    "*** YOUR CODE HERE ***"
    util.raiseNotDefined()

