modded class CraftImprovisedSuppressor extends RecipeBase
{
    override void Init()
    {
        super.Init();

        m_MinQuantityIngredient[1] = 100;
        m_IngredientAddQuantity[1] = -100;
        m_IngredientDestroy[1] = true;
    }
};
